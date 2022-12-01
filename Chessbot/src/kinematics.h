/**
 * @file MotorTask.h
 * @author Dylan Ruiz
 * @brief Motor task that manages actuation for the Motors.
 * @version 1.0
 * @date 2022-11-23
 */

#ifndef _KINEMATICS_H
#define _KINEMATICS_H

#include <Arduino.h>

class Kinematics
{

public:
    Kinematics();
    int16_t coordsToVelocityMotor1(float x_coordinate, float y_coordinate);
    int16_t coordsToVelocityMotor2(float x_coordinate, float y_coordinate);
    uint16_t coordsToStepsMotor1(float x_coordinate, float y_coordinate);
    uint16_t coordsToStepsMotor2(float x_coordinate, float y_coordinate);
};

#endif // _KINEMATICS_H