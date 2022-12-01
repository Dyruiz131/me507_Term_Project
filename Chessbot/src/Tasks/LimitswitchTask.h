/**
 * @file MotorTask.h
 * @author Dylan Ruiz
 * @brief Motor task that manages actuation for the Motors.
 * @version 1.0
 * @date 2022-11-23
 */

#ifndef _LIMIT_SWITCH_TASK_H
#define _LIMIT_SWITCH_TASK_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "objects/MotorDriver.h"

class LimitSwitchTask
{
private:
    uint8_t state;

public:
    LimitSwitchTask();
    void run();
 
};
#endif //_LIMIT_SWITCH_TASK_H