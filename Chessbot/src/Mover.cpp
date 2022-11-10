/**
 * @file Mover_Task.h
 * @author Sam Hudson
 * @brief Mover task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Motor_Driver.h"
#include "Mover.h"

Mover::Mover(Motor *m1, Motor *m2, uint8_t startState)
{
    motor1 = *m1;
    motor2 = *m2;
    state = startState;
}

void start(void *params); // Method for FSM
void setState(uint8_t newState);
void zeroPos();                                   // State 0
void waiting();                                   // State 1
void movePiece(int16_t moveFrom, int16_t moveTo); // State 2
void grabPiece();                                 // State 3
void squareOrigin();                              // State 4
void xGridMove(int16_t x);                        // State 5
void yGridMove(int16_t y);                        // State 6
void gridToCenter();                              // State 7
void releasePiece();                              // State 8
