//
// Created by anfer on 11/16/2019.
//

#include <vector>
#include "process.h"

bool Process::createIOPipes() {
    /*
     * Create and initalizes standard I/O pipes for communicating with the process
     * returns false if initializing the pipes failed.
     * */

    if (pipe(stdinPipe) != 0 || pipe(stdoutPipe) != 0){
        return false;
    }

    return (stdoutPipe[0] > STDERR_FILENO && stdoutPipe[1] > STDERR_FILENO
            && stdinPipe[0] > STDERR_FILENO && stdinPipe[1] > STDERR_FILENO);
}

bool Process::initializeProcess() {
    /*
     * Attempts to start a process using the command.
     * initializeProcess assumes that the process objects are initialized and the standard I/O pipes
     * are created successfully.
     *
     * Returns false if starting the process failed, else true.
     */

    if ((pid = fork()) < 0){
        return false;
    }

    if (pid == 0){
        startProcess();
    }
    else{
        close(stdinPipe[0]);
        close(stdoutPipe[1]);
    }
    return true;
}

void Process::startProcess() {
    fflush(nullptr);

    if (dup2(stdinPipe[0], STDIN_FILENO) < 0 || dup2(stdoutPipe[1], STDOUT_FILENO) < 0){
        // throw exception
    }

    close(stdinPipe[0]);
    close(stdinPipe[1]);
    close(stdoutPipe[0]);
    close(stdoutPipe[1]);

    execv(command, commandLineArgs);
    // throw exception
}



bool Process::Start() {
    /*
     * Attempts to initialize and start the process.
     * returns false if creating the pipes or starting the process failed.
     * */

    if (!createIOPipes()){
        return false;
    }

    return initializeProcess();
}

void Process::Stop() {
    /*
     * Attempts to terminate the process. Does not guarantee that the process has stopped
     * */

    int status;
    if (pid > 0){
        kill(pid, SIGTERM);
        close(stdinPipe[1]);
        close(stdoutPipe[0]);

        waitpid(pid, &status, 0);
    }
}

bool Process::WriteToSTDIN(std::string& data) {

    return write(stdinPipe[1], data.c_str(), data.size()) == data.size();
}

std::string Process::ReadFromSTDOUT() {
    int bytesRead;
    char buffer[BUFSIZE] = {};

    bytesRead = read(stdoutPipe[0], buffer, BUFSIZE-1);

    if( bytesRead <= 0){
        return "";
    }

    std::string dataRead(buffer, (buffer + (bytesRead-1)));
    return dataRead;
}


void Process::setArgs(std::string& cliArgs) {
    char* const* args = splitBySpace(cliArgs);
    this->command = args[0];
    this->commandLineArgs = args;
}

char* const* splitBySpace(std::string& s){
    std::string buffer;
    std::vector<std::string> placeholder;
    std::vector<char *> strPointers;
    std::stringstream sstream(s);
    char * bufferStr = nullptr;

    while (sstream >> buffer){
        placeholder.push_back(buffer);
    }
    strPointers.push_back(const_cast<char *>(placeholder.back().c_str()));
    strPointers.push_back(nullptr);
    return &strPointers[0];
}