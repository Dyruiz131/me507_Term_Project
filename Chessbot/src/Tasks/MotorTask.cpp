/**
 * @file MotorTask.cpp
 * @author Dylan Ruiz
 * @brief Motor task that manages the motor movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "objects/MotorDriver.h"
#include "tasks/MotorTask.h"
#include "shares.h"

MotorTask::MotorTask(Motor motor, Share<bool> &stopMotor, Share<int8_t> &direction, Share<float> &velocity, Share<uint16_t> &steps, Share<bool> &startMotor, Share<bool> &startMotorMax)
{
    motor = motor;
    stopMotor = stopMotor;
    direction = direction;
    velocity = velocity;
    steps = steps;
    startMotor = startMotor;
    startMotorMax = startMotorMax;
    state = 0; // Start state = 0
}

void MotorTask::run() // Method for FSM
{
    switch (state)
    {
    case 0: //Check if the motor should start
    {
        if (startMotor.get())
        {
            state = 1;
            startMotor.put(false);
        }
        else if (startMotorMax.get())
        {
            state = 2;
            startMotorMax.put(false);
        }
        else // Stay in state 0
        {
            state = 0;
        }
        break;
    }
    case 1: //Run motor with specific velocity
    {
        motor.start(velocity.get(), steps.get(), stopMotor);
        state = 0;
        break;
    }
    case 2: //Run motor at max velocity
    {
        motor.startMax(direction.get(), steps.get(), stopMotor);
        state = 0;
        break;
    }
    }
}