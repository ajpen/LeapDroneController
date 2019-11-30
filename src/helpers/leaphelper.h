//
// Created by anfernee on 11/27/19.
//

#ifndef LEAPDRONECONTROLLER_LEAP_H
#define LEAPDRONECONTROLLER_LEAP_H

#include <cmath>
#include "Leap.h"


enum SwipeDirection {
    Left,
    Right,
    Up,
    Down
};


/*
 * Data conversion functions: Below are helpers for handling and converting data from the leap motion controller. */


double radiansToDegrees(float radians){
    return static_cast<double>(radians) * static_cast<double>(180 / M_PI);
}


/*
 * Converts given angle in the expected range 0-180 to a number within the new range specified
 * by rangeStart and rangeEnd
 * */

double normalizeAngleInDegrees(double angle, double rangeStart, double rangeEnd){
    double targetMapper = (rangeEnd - rangeStart) + rangeStart;
    return ((angle - 0) / (180 - 0)) * targetMapper;
}



/*Below are helper functions for simplifying leap motion gestures*/

bool isClockwise(Leap::CircleGesture circleGesture){
    return circleGesture.pointable().direction().angleTo(circleGesture.normal()) <= Leap::PI/2;
}


/*
 * Returns the direction of a swipe within a frame, or null if no gestures are detected
 * */
SwipeDirection GetSwipeDirection(Leap::Frame frame){
    for (auto g: frame.gestures()){
        if (g.type() == Leap::SwipeGesture){
            direction = g.direction().toFloatArray(); // [x, y, z]

            // compares the vertical direction of the swipe with the horizontal direction
            if (direction[1] > direction[0]){
                if (direction[1] > 0){
                    return Up;
                }
                else{
                    return Down;
                }
            }
            else{
                if (direction[0] > 0){
                    return Right;
                }
                else{
                    return Left;
                }
            }
        }
    }
    return nullptr;
}


bool isTakeoffGesture(Leap::Frame frame){
    int numOfHands = 0;

    for (auto hand : frame.hands()){
        if (hand.isValid()){
            numOfHands++;
        }
    }

    // Takeoff only happens when 2 hands are in the frame
    if (numOfHands != 2){
        return false;
    }

    return GetSwipeDirection(frame) == Up;
}

bool isLandingGesture(Leap::Frame frame){
    int numOfHands = 0;

    for (auto hand : frame.hands()){
        if (hand.isValid()){
            numOfHands++;
        }
    }

    // Landing only happens when 2 hands are in the frame
    if (numOfHands != 2){
        return false;
    }

    return GetSwipeDirection(frame) == Down;
}





//bool isTakeoffGesture(Leap::Frame frame){
//    for (auto g: frame.gestures()){
//        if (g.type() == Leap::CircleGesture){
//            return isClockwise(g);
//        }
//    }
//    return false;
//}
//
//
//bool isLandingGesture(Leap::Frame frame){
//    for (auto g: frame.gestures()){
//        if (g.type() == Leap::CircleGesture){
//            return !isClockwise(g);
//        }
//    }
//    return false;
//}

#endif //LEAPDRONECONTROLLER_LEAP_H
