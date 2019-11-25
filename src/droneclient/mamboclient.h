//
// Created by anfernee on 11/22/19.
//

#ifndef LEAPDRONECONTROLLER_PARROTCLIENT_H
#define LEAPDRONECONTROLLER_PARROTCLIENT_H

#include <queue>
#include <thread>
#include <mutex>

#include "../process/process.h"


class MamboFlyClient : Process {
/*
 * Interface to the mambo fly drone client.
 * This class can be used to start the client process
 * and for sending commands/receiving responses to and from
 * the client process.
 *
 * Initialization
 * =====================================================================================================================
 * For initialization, the following configuration keys must exist:
 * command: the full path to the executable and any other required arguments
 * droneAddr: The bluetooth address of the target drone.
 *
 * Optional arguments that may be used to initialize the client process are:
 * maxPitch: The maximum degrees the pitch. Used to control how fast the drone goes forward/backwards (between [-50,50])
 * maxVertical: The maximum speed for vertical movement
 * =====================================================================================================================
 *
 * Controlling The Drone
 * =====================================================================================================================
 * The MamboFlyClient must ensure the process is initialized, connected and ready before sending any flight related
 * commands. To prevent timeout issues, it'll check the connection status and before attempting to takeoff.
 *
 * Attempts to check the connection status will always be synchronous and should be treated as such.
 *
 * Flight instructions sent after takeoff are asynchronous and should be treated as such. These commands are sent to the
 * process, which will queue them for sending to the drone and respond immediately. Because of this, there might be some
 * delay between the command and drone's movements.
 *
 * On takeoff and landing, instructions are ignored until the drone has reported successful takeoff/landing.
 *
 * On any reported failure of the drone, instructions are ignored until the drone is tended to and connection can be
 * re-established. Its very likely that may require restarting the application or initializing a new object, but that
 * depends on how the class is used.
 * =====================================================================================================================
 *
 * Communication
 * =====================================================================================================================
 * The class sends commands through the standard I/O pipes of the process and waits for its acknowledgement of receipt.
 * Synchronous/Asynchronous properties are enforced by the process. The class does expect the process to respond promptly
 * however.
 *
 * Commands
 * =====================================================================================================================
 * The commands sent to the client are as such:
 *
 * safe_takeoff(timeout): Tells drone to take off. This command is synchronous and pauses until the client responds. If
 * the client takes more than 'timeout' seconds to respond, stop trying. Timeout is 20 seconds by default.
 *
 * safe_land(timeout): Tells the drone to land. This command is synchronous and pauses until the client responds.
 *
 * fly_direct(roll, pitch, yaw, vertical_movement): direct flight command for the drone. This command is asynchronous
 * on the client side.
 * */

    private:

        // max seconds to wait on the child to respond
        int processCommunicationTimeout = 500;
        bool clientReady = false;

        // For synchronizing child process and processing thread termination
        const std::string stopSignal = "\tstop\t\n";
        std::mutex processingLock;
        bool isProcessingThreadRunning;

        void setProcessThreadRunning(bool isRunning);
        bool isStillProcessing();


        // Related to the child communication thread (commandProcessingThread)
        std::queue<std::string> pendingCommands;
        std::mutex pendingCommandsLock;
        std::thread* commandProcessingThread = nullptr;

        // Communication protocol methods
        bool waitForRead(int timeoutSeconds);
        bool commandAcknowledged();
        bool connect();

        // command processing methods
        // these methods help sync commands from Leap Motion's event handler, which is concurrent by nature
        void startProcessingCommands();
        std::string safePopFromQueue();
        bool areTherePendingCommands();
        void safePushToQueue(std::string& item);

    public:
        explicit MamboFlyClient(Config& configuration);
        void InitializeAndConnect();
        void SendCommand(std::string command);
        void StopProcessing();


};


#endif //LEAPDRONECONTROLLER_PARROTCLIENT_H
