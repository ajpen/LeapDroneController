//
// Created by anfer on 11/16/2019.
//

#include "process.h"

void Process::initializeProcessAttributes() {

    // Set the bInheritHandle flag so pipe handles are inherited.
    securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.lpSecurityDescriptor = nullptr;
}

bool Process::createIOPipes() {
    /*
     * Create and initalizes standard I/O pipes for communicating with the process
     * returns false if initializing the pipes failed or the read/write handle for
     * STDIN/STDOUT was not inherited (specific to windows. More on this can be found
     * in their documentation.
     * */


    // Create a pipe for the child process's STDOUT.
    if (!CreatePipe(&STDOUT_Reader, &STDOUT_Writer, &securityAttributes, 0)) {
        return false;
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if (!SetHandleInformation(STDOUT_Reader, HANDLE_FLAG_INHERIT, 0)) {
        return false;
    }

    // Create a pipe for the child process's STDIN.
    if (!CreatePipe(&STDIN_Reader, &STDIN_Writer, &securityAttributes, 0)){
        return false;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited.
    if (!SetHandleInformation(STDIN_Writer, HANDLE_FLAG_INHERIT, 0)) {
        return false;
    }
    return true;
}

bool Process::startProcess() {
    /*
     * Attempts to start a process using the command.
     * startProcess assumes that the process objects are initialized and the standard I/O pipes
     * are created successfully.
     *
     * Returns false if CreateProcess failed, else true.
     */

    bool success = false;

    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDIN and STDOUT handles for redirection.
    ZeroMemory(&startupInfo, sizeof(STARTUPINFO));
    startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.hStdError = STDOUT_Writer;
    startupInfo.hStdOutput = STDOUT_Writer;
    startupInfo.hStdInput = STDIN_Reader;
    startupInfo.dwFlags |= STARTF_USESTDHANDLES;

    // create process using Windows API
    success = CreateProcess(nullptr,
                             command,     // command line
                             nullptr,          // process security attributes
                             nullptr,          // primary thread security attributes
                             TRUE,          // handles are inherited
                             0,             // creation flags
                             nullptr,          // use parent's environment
                             nullptr,          // use parent's current directory
                             &startupInfo,  // STARTUPINFO pointer
                             &processInfo);  // receives PROCESS_INFORMATION

    if (!success){
        return false;
    }
    else{
      CloseHandle(processInfo.hProcess);
      CloseHandle(processInfo.hThread);
      return true;
    }
}

void Process::SetCommand(std::string& newCommand) {
    this->command = const_cast<char *>(newCommand.c_str());
}

bool Process::Start() {
    /*
     * Attempts to initialize and start the process.
     * returns false if creating the pipes or starting the process failed.
     * */

    if(strlen(command) < 1){
        return false;
    }

    initializeProcessAttributes();
    if (!createIOPipes()){
        return false;
    }
    return startProcess();
}

void Process::Stop() {
    /*
     * Attempts to terminate the process. Does not guarantee that the process has stopped
     * */

    TerminateProcess(processInfo.hProcess, 0);
}

bool Process::WriteToSTDIN(std::string& data) {
    DWORD bytesWritten;

    return WriteFile(STDIN_Writer, data.c_str(), data.size(), &bytesWritten, nullptr);
}

std::string Process::ReadFromSTDOUT(int numberOfChars) {
    DWORD bytesRead;
    bool success;
    char buffer[BUFSIZE] = {};

    ReadFile(STDOUT_Reader, buffer, BUFSIZE, &bytesRead, nullptr);

    std::string dataRead(buffer, (buffer + (bytesRead-1)));
    return dataRead;
}