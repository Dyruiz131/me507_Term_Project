/**
 * @file MotorTask.cpp
 * @author Dylan Ruiz
 * @brief Motor task that manages the motor movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "shares.h"
#include "Tasks/LimitswitchTask.h"
#define XLIM_PIN 2
#define YLIM_PIN 15

LimitSwitchTask::LimitSwitchTask()
{
 int state = 0;
 startLimitx.put(false);
 startLimity.put(false);
}

void LimitSwitchTask::run() // Method for FSM
{
    switch (state)
    {
    case 0: // waiting for start signal
    {
        if(startLimitx.get())
        {
            state = 1;
            startLimitx.put(false);
        }
        if(startLimity.get())
        {
            state = 2;
            startLimity.put(false);
        }
        break;

    }
    
    case 1: 
    {
       if(digitalRead(XLIM_PIN) == LOW)
       {
           stopMotor1.put(true);
           stopMotor2.put(true);
           state = 0;
       }
     break;
    }
    case 2: 
    {
       if(digitalRead(YLIM_PIN) == LOW)
       {
           stopMotor1.put(true);
           stopMotor2.put(true);
           state = 0;
       }
     break;
    }

    }
}