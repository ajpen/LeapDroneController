//
// Created by anfernee on 11/22/19.
//

#include "mamboclient.h"


MamboFlyClient::MamboFlyClient(Config& configuration) : Process(configuration){
        std::string baseCommand;

        auto droneAddr = configuration.find("droneAddr");
        auto command = configuration.find("command");

        if (command != configuration.end() && droneAddr != configuration.end()){
            baseCommand = command->second;
            baseCommand += " --droneAddr="+ droneAddr->second;
        }
        else{
            throw std::runtime_error("MamboFlyClient: Error, no 'command' or '--droneAddr' configuration set.");;
        }

        // check for addition args
        auto maxPitch = configuration.find("maxPitch");
        auto maxVertical = configuration.find("maxVertical");

        if (maxPitch != configuration.end()){
            baseCommand += " --maxPitch=" + maxPitch->second;
        }

        if (maxVertical != configuration.end()){
            baseCommand += " --maxVertical=" + maxVertical->second;
        }
        droneSate = DroneState{};
        setArgs(baseCommand);
}


/*
 * Waits at upto 'timeoutSeconds' when trying to read from the process's STDOUT
 *
 * returns true if stdout was ready before timeout. Returns false otherwise
 * */
bool MamboFlyClient::waitForRead(int timeoutSeconds) {
    return poll(&stdoutPollFd, 1, timeoutSeconds) > 0;
}

bool MamboFlyClient::commandAcknowledged() {
    std::string response = ReadFromSTDOUT();

    if(response != "ok\n"){
        std::string error = "MamboFlyClient: Unexpected response from child: ";
        error += response;
        throw std::runtime_error(error);
    }
    return true;
}

void MamboFlyClient::InitializeAndConnect(){

    if(!Start()){
        throw std::runtime_error("MamboFlyClient: Failed to start Child Process.");
    }

    if (!connect()){
        throw std::runtime_error("MamboFlyClient: Child unable to connect to drone.");
    }

    clientReady = true;

    // Start the processing thread which should run concurrently
    std::thread processingThread(&MamboFlyClient::startProcessingCommands, this);
    processingThread.detach();
    commandProcessingThread = &processingThread;
}

bool MamboFlyClient::connect() {
    std::string connect = "connect\n";
    WriteToSTDIN(connect);
    return commandAcknowledged();
}

void MamboFlyClient::startProcessingCommands() {
    std::string command;
    setProcessThreadRunning(true);

    while(true){

        // Only read when there are commands available.
        if (!areTherePendingCommands()){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

        } else {
            command = safePopFromQueue();
            if (command == stopSignal) {

                // tell child to stop processing
                WriteToSTDIN(command);

                // Wait until the child finishes processing and cleaning up before exiting.
                if(waitForRead(processCommunicationTimeout + processCommunicationTimeout)){
                    commandAcknowledged();
                    break;
                }
            }

            // newlines terminates the command
            command += '\n';

            if (!WriteToSTDIN(command)) {
                // The pipe is either broke or writing failed somehow.
                // Whatever the case may be, this thread should stop in this situation.
                break;
            }

            // Waits for a read without locking the thread indefinitely
            if (!waitForRead(processCommunicationTimeout)) {
                // there was an error with the polling or the pipe has been closed.
                // Whichever it was, this thread needs to stop
                break;
            }

            if (!commandAcknowledged()) {
                // something happened on the process end. Thread should stop in this situation also
                break;
            }
        }
    }

    // Stops the process and cleans up the file descriptors.
    Stop();

    // signal to any listeners that the thread has stopped
    setProcessThreadRunning(false);
}

std::string MamboFlyClient::safePopFromQueue() {
    pendingCommandsLock.lock();
    auto item = pendingCommands.front();
    pendingCommands.pop();
    pendingCommandsLock.unlock();
    return item;
}

void MamboFlyClient::safePushToQueue(std::string& item) {
    pendingCommandsLock.lock();
    pendingCommands.push(item);
    pendingCommandsLock.unlock();
}

void MamboFlyClient::SendCommand(std::string command) {
    safePushToQueue(command);
}

/*
 * Returns true if there are items in the pending commands Queue
 * Thread safe.
 * */
bool MamboFlyClient::areTherePendingCommands() {
    bool pending;
    pendingCommandsLock.lock();
    pending = pendingCommands.empty();
    pendingCommandsLock.unlock();
    return !pending;
}

void MamboFlyClient::StopProcessing() {
    SendCommand(stopSignal);

    // wait for the processing thread to end
    while (isStillProcessing()){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

bool MamboFlyClient::isStillProcessing() {
    bool isProcessing;
    processingLock.lock();
    isProcessing = isProcessingThreadRunning;
    processingLock.unlock();
    return isProcessing;
}

void MamboFlyClient::setProcessThreadRunning(bool isRunning) {
    processingLock.lock();
    isProcessingThreadRunning = isRunning;
    processingLock.unlock();
}

void MamboFlyClient::Takeoff(int timeout) {
    std::string command = "takeoff ";
    SendCommand(command + std::to_string(timeout));

}

void MamboFlyClient::Land(int timeout) {
    std::string command = "land ";
    SendCommand(command + std::to_string(timeout));
}

void MamboFlyClient::Fly(int roll, int pitch, int yaw, int vertical_movement) {
    std::string command = "fly ";
    command += std::to_string(roll) + " " + std::to_string(pitch) + " " +
            std::to_string(yaw) + " " + std::to_string(vertical_movement);

    SendCommand(command);
}


bool DroneState::isInFlight() {
    bool inFlight;
    droneStateLock.lock();
    inFlight = flightState;
    droneStateLock.unlock();
    return inFlight;
}

void DroneState::setFlightState(bool currentFlightState) {
    droneStateLock.lock();
    flightState = currentFlightState;
    droneStateLock.unlock();
}

bool DroneState::isConnected() {
    bool connected;
    droneStateLock.lock();
    connected = connectionStatus;
    droneStateLock.unlock();
    return connected;
}

void DroneState::setConnectionStatus(bool isCurrentlyConnected) {
    droneStateLock.lock();
    connectionStatus = isCurrentlyConnected;
    droneStateLock.unlock();
}

void DroneState::incrementIdleFrames() {
    droneStateLock.lock();
    idleFrames++;
    droneStateLock.unlock();
}

bool DroneState::isIdle(float framerate) {
    bool isDroneIdle;
    droneStateLock.lock();
    isDroneIdle = static_cast<double>(static_cast<double>(idleFrames)/framerate) > 5;
    droneStateLock.unlock();
    return isDroneIdle;
}
