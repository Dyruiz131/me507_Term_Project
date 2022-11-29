/**
 * @file MotorTask.cpp
 * @author Dylan Ruiz
 * @brief Motor task that manages the motor movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Objects/MotorDriver.h"
#include "Tasks/MotorTask.h"

#define Solenoid_Pin 11

/**
 *
 * @brief Construct a new Motor Task object
 *
 * @param motor1 Motor object
 * @param stopMotor1 Share object used as a flag for stopping the motor
 * @param direction1 Share object used to indicate direction of the motor
 * @param velocity1 Share object used to input velocity for motor function
 * @param steps1 Share object used to input number of steps for motor function
 * @param Motor_start1 Share object used as a flag to progress FSM to state 1
 * @param MotorMax_Start1 Share object used as a flag to progress FSM to state 2
 */

MotorTask::MotorTask(Motor m, Share<bool> &stopM, Share<int8_t> &dir, Share<float> &vel, Share<uint16_t> &st, Share<bool> &startM, Share<bool> &startMMax)
{
    motor = m;
    stopMotor = stopM;
    direction = dir;
    velocity = vel;
    steps = st;
    startMotor = startM;
    startMotorMax = startMMax;
    state = 0; // Start state = 0
    motor.enable(true);
}

/**
 * @brief Method called for multitasking. This controls the Controller FSM.
 */
void MotorTask::run() // Method for FSM
{

    switch (state)
    {
    case 0:
    {
        waiting();
        if (startMotor.get() == true)
        {
            state = 1; //
            startMotor.put(false);
        }
        else if (startMotorMax.get() == true)
        {
            state = 2; // Scan Board State
            startMotorMax.put(false);
        }
        break;
    }
    case 1:
    {
        runMotor();
        state = 0;
        break;
    }
    case 2:
    {
        runMotorMax();
        state = 0;
        break;
    }
    }
}

void MotorTask::waiting() // Waiting
{
}
void MotorTask::runMotor() // State 2
{
    motor.velocity(velocity.get(), steps.get(), stopMotor);
}
void MotorTask::runMotorMax() // State 3
{
    motor.maxVelocity(direction.get(), steps.get(), stopMotor);
}
