/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include <iostream>

#include "Leap.h"
#include "droneclient/mamboclient.h"
#include "configuration/parser.h"
#include "helpers/leaphelper.h"

using namespace Leap;


// Change this to be a Mambo fly drone specific listener.
// Store the client apart of the listener and then access that
// through the class members.
class DroneControllerListener : public Listener {

private:
    MamboFlyClient* client = nullptr;

public:
    explicit DroneControllerListener(MamboFlyClient* droneClient): Listener(){
        client = droneClient;
    }
    void onConnect(const Controller&) override;
    void onFrame(const Controller&) override;
};

void DroneControllerListener::onConnect(const Controller& controller) {
    // Enable the swipe gesture for takeoff/landing
    controller.enableGesture(Gesture::TYPE_SWIPE);
    controller.enableGesture(Gesture::TYPE_CIRCLE);
}

void DroneControllerListener::onFrame(const Controller& controller) {

    const Frame frame = controller.frame();

    // Check for takeoff gesture
    if (isTakeoffCircleGesture(frame)){
        client->Takeoff();
//        std::cout << "Takeoff called" << std::endl;
        return;
    }

    // Check for landing gesture
    if (isLandingCircleGesture(frame)){
        client->Land();
//        std::cout << "Land called" << std::endl;
        return;
    }

    auto hands = frame.hands();

    // Only detect instructions when the accuracy is at least 80% and all 5 fingers are extended
    if (hands.count() == 1 && hands[0].confidence() >= 0.9 && hands[0].fingers().extended().count() == 5){

        // Detect palm orientation and send it to the drone
        auto pitchDegrees = radiansToDegrees(hands[0].direction().pitch());
        auto pitch = normalizeAngleInDegrees(
                pitchDegrees, 0, client->maxPitchAngle) * -1;

        auto yaw = radiansToDegrees(hands[0].direction().yaw());
        auto roll = radiansToDegrees(hands[0].palmNormal().roll());

        double verticalMovement = 0;

        // detect vertical change since last frame
        auto previousFrame = controller.frame(1);

        auto prevHands = previousFrame.hands();

        if (prevHands.count() == 1) {
            verticalMovement = hands[0].palmPosition().y - prevHands[0].palmPosition().y;
        }

        client->Fly(roll, pitch, yaw, verticalMovement);
//        std::cout << "Fly called with the values for roll: " << roll << " pitch: " << pitch << " yaw: " << yaw
//                << " and vertical_movement: " << verticalMovement << std::endl;
        return;
    }
}


int main(int argc, char** argv) {

    std::string configPath = "config.txt";

    Controller controller;
    ConfigParser parser;

    auto config = parser.Parse(configPath);

    MamboFlyClient client(config);
    DroneControllerListener listener(&client);


    controller.addListener(listener);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
