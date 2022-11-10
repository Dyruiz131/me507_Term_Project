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
Mover::Mover(Motor m1, Motor m2, uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN)
{
    motor1 = m1;
    motor2 = m2;
    state = 0; // Start state = 0
    uint16_t xStep = 0;
    uint16_t yStep = 0;
    bool scanBoard = false;
    uint8_t xLimPin = XLIM_PIN;
    uint8_t yLimPin = YLIM_PIN;
    uint8_t solenoidPin = SOLENOID_PIN;
}
/**
 * @brief Method called for multitasking. This controls the Mover FSM.
 */
void Mover::run() // Method for FSM
{
    switch (state)
    {
    case 0:
    {
        origin();
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

void Mover::origin() // State 0
{
    /* Check x axis */
    while (1)
    {
        if (!digitalRead(xLimPin)) // If x lim switch is hit, stop moving.
        {
            xStep = 0; // Set x position to 0
            break;     // End while loop
        }
        else
        {
            // Move left 1 step
            motor1.Velocity_MAX(-1, 1);
            motor2.Velocity_MAX(-1, 1);
        }
    }
    /* Check y axis */
    while (1)
    {
        if (!digitalRead(yLimPin)) // If y lim switch is hit, stop moving.
        {
            yStep = 0; // Set y position to 0
            break;     // End while loop
        }
        else
        {
            // Move down 1 step
            motor1.Velocity_MAX(1, 1);
            motor2.Velocity_MAX(-1, 1);
        }
    }
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
