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
#include "objects/MotorDriver.h"

class MotorTask
{
private:
    Motor motor;
    uint8_t state;

public:
    MotorTask(Motor m, Share<bool> &stopM, Share<int8_t> &dir,
              Share<float> &vel, Share<uint16_t> &st, Share<bool> &startM,
              Share<bool> &startMMax);
    Share<bool> stopMotor;
    Share<int8_t> direction;
    Share<float> velocity;
    Share<uint16_t> steps;
    Share<bool> startMotor;
    Share<bool> startMotorMax;
    void run();
    void waiting();
    void runMotor();    // Method for velocity specific motor actuation
    void runMotorMax(); // Method for Max velocity motor actuation
};

#endif // _MOTORTASK_H