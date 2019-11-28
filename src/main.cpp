/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>
#include <cstring>
#include "Leap.h"

using namespace Leap;


// Change this to be a Mambo fly drone specific listener.
// Store the client apart of the listener and then access that
// through the class members.
class SampleListener : public Listener {
public:
    void onConnect(const Controller&) override;
    void onFrame(const Controller&) override;
};

void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Leap Motion Connected." << std::endl;
}

void SampleListener::onFrame(const Controller& controller) {

}


int main(int argc, char** argv) {
    SampleListener listener;
    Controller controller;

    // init client
    // add code to listener to recognise gestures and send commands
    // define fly, takeoff and land

    controller.addListener(listener);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);
    return 0;
}
