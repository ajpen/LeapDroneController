//
// Created by anfernee on 11/27/19.
//

#ifndef LEAPDRONECONTROLLER_LEAP_H
#define LEAPDRONECONTROLLER_LEAP_H

#include <cmath>
#include "Leap.h"


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



#endif //LEAPDRONECONTROLLER_LEAP_H
