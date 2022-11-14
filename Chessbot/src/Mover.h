/**
 * @file Mover.h
 * @author Sam Hudson
 * @brief Mover task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#ifndef _MOVER_TASK_H
#define _MOVER_TASK_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Motor_Driver.h"

class Mover
{
private:
    Motor motor1;
    Motor motor2;
    uint8_t state;
    uint16_t xStep;
    uint16_t yStep;
    bool scanBoard;
    uint8_t xLimPin;
    uint8_t yLimPin;
    uint8_t solenoidPin;

public:
    Mover(Motor m1, Motor m2, uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN); // Constructor
    void run();                                                                          // Method for FSM
    void setState(uint8_t newState);
    void origin();                                    // State 0
    void waiting();                                   // State 1
    void movePiece(int16_t moveFrom, int16_t moveTo); // State 2
    void grabPiece();                                 // State 3
    void squareOrigin();                              // State 4
    void xGridMove(int16_t x);                        // State 5
    void yGridMove(int16_t y);                        // State 6
    void gridToCenter();                              // State 7
    void releasePiece();                              // State 8
    void limSwitchRedirection();                      // State 9
    void scanBoard();                                 // State 10
};

#endif // _MOVER_TASK_H
