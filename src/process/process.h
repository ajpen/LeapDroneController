//
// Created by anfer on 11/16/2019.
/*
* Code for starting and managing processes
*/

#ifndef LEAPDRONECONTROLLER_PROCESS_H
#define LEAPDRONECONTROLLER_PROCESS_H

#include <cstdio>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <sys/wait.h>
#include <cstdarg>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <poll.h>
#include <fstream>
#include <cerrno>

#include "../configuration/config.h"


#define PIPE_READ 0
#define PIPE_WRITE 1
#define BUFSIZE 512


class Process {
    /*
     * Process is a small interface for starting and managing a running process
     *
     * It provides methods for reading and writing to the standard I/O streams of
     * the running process, and methods for managing the process (such as starting
     * and stopping it)
     * */

    private:

        /*attributes*/

        // Pipes
        int stdinPipe[2] = {};
        int stdoutPipe[2] = {};

        // Process PID
        pid_t pid = -1;

        // program name/path
        const char* command = nullptr;

        // Full command line argument
        std::vector<char*> commandLineArgs;
        std::vector<std::string> stringCommandLineArgs;

        /*Methods*/
        bool createIOPipes();
        bool initializeProcess();
        void startProcess();

    protected:
    // Attributes
    struct pollfd stdoutPollFd = {.fd = stdoutPipe[0], .events = POLLIN};

    void setArgs(std::string& cliArgs);

    public:
		explicit Process(std::string args) {
            setArgs(args);
        }

        explicit Process(Config& configuration){
		    auto commandGiven = configuration.find("command");
		    if ( commandGiven != configuration.end()){
		        setArgs(commandGiven->second);
		    }
		    else{
                throw std::runtime_error("Process: Error, no 'command' configuration set.");
		    }
		}

        bool Start();
        void Stop();
        bool WriteToSTDIN(std::string& data);
        std::string ReadFromSTDOUT();
};

#endif //LEAPDRONECONTROLLER_PROCESS_H
