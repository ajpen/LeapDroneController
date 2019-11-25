//
// Created by anfernee on 11/23/19.
//

#include "mamboclient.h"
#include "../configuration/parser.h"

int main(){
    std::string configPath = "config.txt";
    std::string command1 = "safe_takeoff(timeout)";
    std::string command2 = "fly_direct(roll, pitch, yaw, vertical_movement)";
    std::string command3 = "safe_land(timeout)";

    ConfigParser parser;
    auto config = parser.Parse(configPath);

    MamboFlyClient client(config);
    client.InitializeAndConnect();
    client.SendCommand(command1);
    client.SendCommand(command2);
    client.SendCommand(command3);
    client.StopProcessing();
}