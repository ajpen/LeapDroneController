//
// Created by anfer on 11/18/2019.
//

#include <iostream>
#include "process.h"

using namespace std;

int main(int argc, char* argv[]){

    string s = "python3 child.py";
    string expected = "I received something\n";
    string data = "something\n";


    Process process(s);
    if (!process.Start()){
        cout << "Failed to start process." << endl;
        exit(1);
    }

    if (!process.WriteToSTDIN(data)){
        cout << "Failed to write to stdin." << endl;
        exit(1);
    }
    string recv = process.ReadFromSTDOUT();

    if (recv != expected){
        cout << "expected '" << expected << "' from process but got " << recv << endl;
        exit(1);
    }

    cout << "Passed" << endl;
}