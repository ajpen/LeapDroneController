//
// Created by anfer on 11/16/2019.
/*
* Code for starting and managing the process is inspired by this recipe on Microsoft's
* documentation:
* https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output
*/

#ifndef LEAPDRONECONTROLLER_PROCESS_H
#define LEAPDRONECONTROLLER_PROCESS_H

#include <cstdio>
#include <string>
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#define BUFSIZE 512


class Process {
    /*
     * Process is a small interface for starting and managing a running process using the windows API.
     *
     * It provides methods for reading and writing to the standard I/O streams of
     * the running process, and methods for managing the process (such as starting
     * and stopping it)
     * */

    private:

        /*attributes?*/

        // Process information used by Window's "StartProcess"
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInfo;

        // Elements for reading and writing to Standard I/O
        HANDLE STDIN_Reader = nullptr;
        HANDLE STDIN_Writer = nullptr;
        HANDLE STDOUT_Reader = nullptr;
        HANDLE STDOUT_Writer = nullptr;

        // required for creating I/O pipes
        SECURITY_ATTRIBUTES securityAttributes;

        // full command line argument, including program name/path
        char * command = nullptr;

        /*Methods*/
        void initializeProcessAttributes();
        bool createIOPipes();
        bool startProcess();

    public:
        explicit Process(const std::string &command){this->command = const_cast<char *>(command.c_str());}
		explicit Process(const char* command) { this->command = const_cast<char *>(command);}

        void SetCommand(std::string& newCommand);
        bool Start();
        void Stop();
        bool WriteToSTDIN(std::string& data);
        std::string ReadFromSTDOUT(int numberOfChars);
};

#endif //LEAPDRONECONTROLLER_PROCESS_H
