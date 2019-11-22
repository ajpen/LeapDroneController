//
// Created by anfer on 11/18/2019.
//

#include <iostream>
#include "process.h"

using namespace std;

int main(int argc, char* argv[]){

    const char * command = "python";
    const char * args[2] = {"python","child.py"};
    string expected = "I received something";
    string data = "something";


    // TODO: Figure out a way to pass a char* array as the arguments to execv
    Process process(command, args);
    if (!process.Start()){
        cout << "Failed to start process." << endl;
        exit(1);
    }

    if (!process.WriteToSTDIN(data)){
        cout << "Failed to write to stdin." << endl;
        exit(1);
    }
    string recv = process.ReadFromSTDOUT(BUFSIZE);

    if (recv == expected){
        cout << "expected " << expected << "from process but got " << recv << endl;
        exit(1);
    }

    cout << "Passed" << endl;
}