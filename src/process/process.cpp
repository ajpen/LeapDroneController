//
// Created by anfer on 11/16/2019.
//

#include "process.h"

bool Process::createIOPipes() {
    /*
     * Create and initalizes standard I/O pipes for communicating with the process
     * returns false if initializing the pipes failed.
     * */

    if (pipe(stdinPipe) != 0 || pipe(stdoutPipe) != 0){
        return false;
    }

    // set Pollfd to stdout
    stdoutPollFd.fd = stdoutPipe[0];

    return (stdoutPipe[PIPE_READ] > STDERR_FILENO && stdoutPipe[PIPE_WRITE] > STDERR_FILENO
            && stdinPipe[PIPE_READ] > STDERR_FILENO && stdinPipe[PIPE_WRITE] > STDERR_FILENO);
}

bool Process::initializeProcess() {
    /*
     * Attempts to start a process using the command.
     * initializeProcess assumes that the process objects are initialized and the standard I/O pipes
     * are created successfully.
     *
     * Returns false if starting the process failed, else true.
     */

    pid = fork();
    if (pid == 0){
        startProcess();
    }

    else if (pid > 0){
        close(stdinPipe[PIPE_READ]);
        close(stdoutPipe[PIPE_WRITE]);
    }
    else{
        return false;
    }
    return true;

}

void Process::startProcess() {
    fflush(nullptr);

    if (dup2(stdinPipe[PIPE_READ], STDIN_FILENO) < 0 || dup2(stdoutPipe[PIPE_WRITE], STDOUT_FILENO) < 0){
        // TODO: throw some exception
        throw;
    }

    close(stdinPipe[PIPE_READ]);
    close(stdinPipe[PIPE_WRITE]);
    close(stdoutPipe[PIPE_READ]);
    close(stdoutPipe[PIPE_WRITE]);

    execvp(command, &const_cast<char **>(commandLineArgs.data())[0]);

// I should use this to do some logging later on
//    std::ofstream f;
//    f.open ("child.log");
//    f << "Child execv failed: " << strerror(errno);
//    f.close();
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
        close(stdinPipe[PIPE_WRITE]);
        close(stdoutPipe[PIPE_READ]);

        waitpid(pid, &status, 0);
    }
}

/*
 * Writes data as is to STDIN of the process.
 *
 * Note that the child may continue waiting on read data isn't terminated
 * by a newline character. This is specific to the child.
 * */
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

    std::string dataRead(buffer, (buffer + (bytesRead)));
    return dataRead;
}


/*
 * splits the command line arguments (cliArgs) by space and prepares them for execv()
 * */
void Process::setArgs(std::string& cliArgs) {

    // Reset any previous commands
    commandLineArgs.clear();
    stringCommandLineArgs.clear();

    std::size_t start = 0, end = 0;
    while ((end = cliArgs.find(' ', start)) != std::string::npos) {
        stringCommandLineArgs.push_back(cliArgs.substr(start, end - start));
        start = end + 1;
    }
    stringCommandLineArgs.push_back(cliArgs.substr(start));
    for (auto & stringCommandLineArg : stringCommandLineArgs) {
        commandLineArgs.push_back(const_cast<char *>(stringCommandLineArg.c_str()));
    }

    commandLineArgs.push_back(nullptr);

    // set the command string (the first cli argument)
    command = commandLineArgs[0];

}
