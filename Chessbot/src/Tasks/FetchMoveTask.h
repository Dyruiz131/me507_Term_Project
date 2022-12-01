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
    APIHandler api;
    uint8_t state;

public:
    FetchMoveTask(APIHandler api);
    FetchMoveTask(); // Default constructor
    void run(); // Method for FSM
};

#endif // _FETCH_MOVE_TASK_H_