/**
 * @file MotorTask.cpp
 * @author Dylan Ruiz
 * @brief Provides an FSM for a motor
 * @version 1.0
 * @date 2022-11-23
 */

#include <Arduino.h>
#include "objects/MotorDriver.h"
#include "tasks/MotorTask.h"
#include "shares.h"

/**
 * @brief Construct a new Motor Task object
 *
 * @param motor The motor object that will be controlled by the FSM
 * @param stopMotor shared variable used to stop motor
 * @param direction shared variable used to indicate direction of motor for motorstartMax method
 * @param velocity shared variable used to give velocity of motor for motorstart method
 * @param steps shared variable indicating number of steps motor should take
 * @param startMotor shared variable acting as a flag used to start the motor using the method startMotor
 * @param startMotorMax shared variable acting as a flag used to start the motor using the method startMotorMax
 */
MotorTask::MotorTask(Motor motor, Share<bool> &stopMotor, Share<int8_t> &direction, Share<float> &velocity, Share<uint16_t> &steps, Share<bool> &startMotor, Share<bool> &startMotorMax)
{
    this->motor = motor;
    this->stopMotor = stopMotor;
    this->direction = direction;
    this->velocity = velocity;
    this->steps = steps;
    this->startMotor = startMotor;
    this->startMotorMax = startMotorMax;
    state = 0; // Start state = 0
    startMotor.put(false);
    startMotorMax.put(false);
}

/**
 * @brief Run the FSM for the motor
 * 
 */
void MotorTask::run() // Method for FSM
{
    switch (state)
    {
    case 0: // Check if the motor should start
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
    case 1: // Run motor with specific velocity
    {
        Serial.println("Motor started");
        stopMotor.put(false);
        motor.start(velocity.get(), steps.get(), stopMotor);

        state = 0;
        break;
    }
    case 2: // Run motor at max velocity
    {
        motor.startMax(direction.get(), steps.get(), stopMotor);
        state = 0;
        break;
    }
    }
}