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
    Down,
    Invalid
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


/*
 * Converts value from the leap motion vector (whose range is 25-600 mm) to a number
 * within a specified range
 * */
double normalizeNumber(double value, double rangeStart, double rangeEnd){
    double targetMapper = (rangeEnd - rangeStart) + rangeStart;
    return ((value - 0) / (600 - 25)) * targetMapper;
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
        if (g.type() == Leap::Gesture::TYPE_SWIPE){
            auto swipe = Leap::SwipeGesture(g);
            auto direction = swipe.direction(); // [x, y, z]

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
    return Invalid;
}


bool isTakeoffGesture(const Leap::Frame& frame){
    int numOfHands = frame.hands().count();

    // Takeoff only happens when 2 hands are in the frame
    if (numOfHands != 2){
        return false;
    }

    return GetSwipeDirection(frame) == Up;
}

bool isLandingGesture(const Leap::Frame& frame){
    int numOfHands = frame.hands().count();

    // Landing only happens when 2 hands are in the frame
    if (numOfHands != 2){
        return false;
    }

    return GetSwipeDirection(frame) == Down;
}


bool isTakeoffCircleGesture(const Leap::Frame& frame){
    for (auto g: frame.gestures()){
        if (g.type() == Leap::Gesture::TYPE_CIRCLE){
            auto circleG = Leap::CircleGesture(g);
            if (circleG.progress() > 1){
                return isClockwise(circleG);
            }
        }
    }
    return false;
}


bool isLandingCircleGesture(const Leap::Frame& frame){
    for (auto g: frame.gestures()){
        if (g.type() == Leap::Gesture::TYPE_CIRCLE){
            auto circleG = Leap::CircleGesture(g);

            if (circleG.progress() > 1){
                return !isClockwise(circleG);
            }
        }
    }
    return false;
}

#endif //LEAPDRONECONTROLLER_LEAP_H
