/**
 * @file MotorTask.h
 * @author Dylan Ruiz
 * @brief Motor task that manages actuation for the Motors.
 * @version 1.0
 * @date 2022-11-23
 */

#ifndef _MOTORTASK_H
#define _MOTORTASK_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Objects/MotorDriver.h"

class MotorTask
{
private:
    Motor motor;
    uint8_t state;
    Share<bool> stopMotor;
    Share<int8_t> direction;
    Share<float> velocity;
    Share<uint16_t> steps;
    Share<bool> startMotor;
    Share<bool> startMotorMax;

public:
    MotorTask(Motor motor1, Share<bool> &stopMotor, Share<int8_t> &direction,
              Share<float> &velocity, Share<uint16_t> &steps, Share<bool> &startMotor,
              Share<bool> &startMotorMax); // Constructor
    void run();
    void waiting();
    void runMotor();    // Method for velocity specific motor actuation
    void runMotorMax(); // Method for Max velocity motor actuation
};

#endif // _MOTORTASK_H