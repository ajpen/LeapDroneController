//
// Created by anfer on 11/16/2019.
/*
* Code for starting and managing processes
*/

#ifndef LEAPDRONECONTROLLER_PROCESS_H
#define LEAPDRONECONTROLLER_PROCESS_H

#include <cstdio>
#include <string>
#include <cstring>
#include <sys/wait.h>
#include <cstdarg>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>


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

        /*attributes?*/

        // Pipes
        int stdinPipe[2] = {};
        int stdoutPipe[2] = {};

        // Process PID
        pid_t pid = -1;

        // program name/path
        const char * command = nullptr;

        // Full command line argument
        char *const *commandLineArgs = nullptr;

        /*Methods*/
        bool createIOPipes();
        bool initializeProcess();
        void startProcess();

    public:
//        explicit Process(const std::string &command){this->command = const_cast<char *>(command.c_str());}
		explicit Process(const char* command, char *const *clArguments) {
            this->command = command;
            this->commandLineArgs = reinterpret_cast<char *const *>(clArguments);
        }

        bool Start();
        void Stop();
        bool WriteToSTDIN(std::string& data);
        std::string ReadFromSTDOUT();
};

#endif //LEAPDRONECONTROLLER_PROCESS_H
