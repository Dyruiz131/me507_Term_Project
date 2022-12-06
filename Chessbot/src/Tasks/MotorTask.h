/**
 * @file MotorTask.h
 * @author Dylan Ruiz
 * @brief Motor task that manages actuation for the Motors.
 * @version 1.0
 * @date 2022-11-23
 */

#ifndef _MOTOR_TASK_H
#define _MOTOR_TASK_H

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
    MotorTask(Motor m, Share<bool> &stopM, Share<int8_t> &dir, Share<float> &vel, Share<uint16_t> &step, Share<bool> &startM, Share<bool> &startMMax);
    Share<bool> stopMotor;
    Share<int8_t> direction;
    Share<float> velocity;
    Share<uint16_t> steps;
    Share<bool> startMotor;
    Share<bool> startMotorMax;
    void run();
};

#endif // _MOTOR_TASK_H