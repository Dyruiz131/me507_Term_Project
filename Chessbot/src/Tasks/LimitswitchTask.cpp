/**
 * @file LimitSwitchTask.cpp
 * @author Dylan Ruiz
 * @brief Provides the FSM for checking the limit switches
 * @version 1.0
 * @date 2022-11-23
 */

#include <Arduino.h>
#include "shares.h"
#include "tasks/LimitSwitchTask.h"

/**
 * @brief Construct a new Limit Switch Task
 *
 * @param XLIM_PIN X limit switch pin
 * @param YLIM_PIN Y limit switch pin
 */
LimitSwitchTask::LimitSwitchTask(uint8_t XLIM_PIN, uint8_t YLIM_PIN)
{
    int state = 0;
    this->XLIM_PIN = XLIM_PIN;
    this->YLIM_PIN = YLIM_PIN;
    startLimitx.put(false);
    startLimity.put(false);
}
/**
 * @brief Run the FSM for the limit switches
 *
 */
void LimitSwitchTask::run() // Method for FSM
{
    switch (state)
    {
    case 0: // waiting for start signal
    {
        if (startLimitx.get())
        {
            state = 1;
            startLimitx.put(false);
        }
        if (startLimity.get())
        {
            state = 2;
            startLimity.put(false);
        }
        break;
    }

    case 1:
    {
        if (digitalRead(XLIM_PIN) == LOW)
        {
            stopMotor1.put(true);
            stopMotor2.put(true);
            state = 0;
        }
        break;
    }
    case 2:
    {
        if (digitalRead(YLIM_PIN) == LOW)
        {
            stopMotor1.put(true);
            stopMotor2.put(true);
            state = 0;
        }
        break;
    }
    }
}