//
// Created by anfernee on 11/29/19.
//

#include <iostream>

#include "Leap.h"
#include "droneclient/mamboclient.h"
#include "configuration/parser.h"
#include "helpers/leaphelper.h"

using namespace Leap;


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
    controller.enableGesture(Gesture::TYPE_CIRCLE);
}

void DroneControllerListener::onFrame(const Controller& controller) {

    const Frame frame = controller.frame();

    // Check for takeoff gesture
    if (isTakeoffCircleGesture(frame)){
        client->Takeoff();
        return;
    }

    // Check for landing gesture
    if (isLandingCircleGesture(frame)){
        client->Land();
        return;
    }

    // Frame skipping
    if (frame.id() % 5 != 0){
        return;
    }

    auto hands = frame.hands();

    // Only detect instructions when the accuracy is at least 80% and all 5 fingers are extended
    if (hands.count() == 1 && hands[0].confidence() >= 0.96 && hands[0].fingers().extended().count() == 5){

        // Detect palm orientation and send it to the drone
        auto pitchDegrees = radiansToDegrees(hands[0].direction().pitch()) * -1;
        auto pitch = normalizeAngleInDegrees(
                pitchDegrees, 0, client->maxPitchAngle);

        auto yaw = normalizeAngleInDegrees(
                radiansToDegrees(hands[0].direction().yaw()), 0, client->maxPitchAngle);


        auto roll = normalizeAngleInDegrees(
                radiansToDegrees(hands[0].palmNormal().roll()), 0, client->maxPitchAngle);

        double verticalMovement = 0;

        // detect vertical change since last frame
        auto previousFrame = controller.frame(1);

        auto prevHands = previousFrame.hands();

        if (prevHands.count() == 1) {
            verticalMovement = hands[0].palmPosition().y - prevHands[0].palmPosition().y;
        }

        client->Fly(roll, pitch, yaw, verticalMovement);
    }
}


int main(int argc, char** argv) {

    std::string configPath = "config.txt";

    Controller controller;
    ConfigParser parser;

    auto config = parser.Parse(configPath);

    MamboFlyClient client(config);
    client.InitializeAndConnect();

    DroneControllerListener listener(&client);

    controller.addListener(listener);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    client.StopProcessing();

    return 0;
}
