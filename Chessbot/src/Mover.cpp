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
/**
 * @brief Construct a new Mover object
 *
 * @param m1 Motor 1 object
 * @param m2 Motor 2 object
 * @param XLIM_PIN Pin for x limit switch
 * @param YLIM_PIN Pin for y limit switch
 */
Mover::Mover(Motor m1, Motor m2, uint8_t XLIM_PIN, uint8_t YLIM_PIN)
{
    motor1 = m1;
    motor2 = m2;
    state = 0; // Start state = 0
    uint16_t motor1Pos = 0;
    uint16_t motor2Pos = 0;
    bool scanBoard = false;
    uint8_t xLimPin = XLIM_PIN;
    uint8_t yLimPin = YLIM_PIN;
}
/**
 * @brief Method called for multitasking. This controls the Mover FSM.
 *
 * @param params Void pointer for FreeRTOS setup
 */
void Mover::run() // Method for FSM
{
    switch (state)
    {
    case 0:
    {
        zeroPos();
        break;
    }
    case 1:
    {
        waiting();
        break;
    }
    case 2:
    {
        movePiece(0, 0); // Need to add in a share for getting these vals
        break;
    }
    case 3:
    {
        grabPiece();
        break;
    }
    case 4:
    {
        squareOrigin();
        break;
    }
    case 5:
    {
        xGridMove(0); // Share val needed here
        break;
    }
    case 6:
    {
        yGridMove(0); // Share val needed here
        break;
    }
    case 7:
    {
        gridToCenter();
        break;
    }
    case 8:
    {
        releasePiece();
        state = 0;
        break;
    }
    case 9:
    {
        limSwitchRedirection();
        state = 10;
        break;
    }
    case 10:
    {
        scanBoard();
        state = 0;
        break;
    }
    }
}

void Mover::setState(uint8_t newState)
{
    state = newState;
}

void Mover::zeroPos() // State 0
{
}
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

/* Helper methods */
