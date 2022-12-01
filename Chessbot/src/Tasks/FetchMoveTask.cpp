/**
 * @file MotorTask.cpp
 * @author Dylan Ruiz
 * @brief Motor task that manages the motor movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "tasks/FetchMoveTask.h"
#include "shares.h"

/**
 * @brief Construct a new FetchMoveTask object
 *
 * @param api The API handler object
 */
FetchMoveTask::FetchMoveTask(APIHandler api)
{
    this->api = api;
    state = 0; // Start state = 0
}

/**
 * @brief Default constructor necessary for dependency injection
 */
FetchMoveTask::FetchMoveTask()
{
}

void FetchMoveTask::run(){
    while(true){
        switch(state){
            case 0:
        }
    }
}