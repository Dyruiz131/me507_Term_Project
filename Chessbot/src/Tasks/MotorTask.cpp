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
#include "objects/MotorDriver.h"
#include "tasks/MotorTask.h"
#include "shares.h"

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
