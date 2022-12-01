/**
 * @file FetchMoveTask.h
 * @author Sam Hudson
 * @brief Task that operates the API handler
 * @version 1.0
 * @date 2022-11-30
 */

#ifndef _FETCH_MOVE_TASK_H_
#define _FETCH_MOVE_TASK_H_

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "objects/APIHandler.h"

class FetchMoveTask
{
private:
    Motor motor;
    uint8_t state;

public:
    FetchMoveTask(APIHandler api);
    Queue<float> directionsQueue;
    Share<bool> beginMove;
    void run(); // Method for FSM
    void waiting();
    void runMotor(); 
    void runMotorMax();
};

#endif // _FETCH_MOVE_TASK_H_