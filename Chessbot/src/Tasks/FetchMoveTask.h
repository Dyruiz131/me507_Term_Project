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

class FetchMove
{
private:
    APIHandler api;
    uint8_t state;
    String lastMove;
    String newMove;
    float gridCoordinates[8];

public:
    FetchMove(APIHandler api);
    FetchMove(); // Default constructor
    void run();  // Method for FSM
    void moveDone();
    void moveFailed();
    float toCoordinate(char col);
    float toCoordinate(uint8_t row);
    float *gridLocations[8];
};

#endif // _FETCH_MOVE_TASK_H_