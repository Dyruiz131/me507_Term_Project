/**
 * @file Mover_Task.h
 * @author Sam Hudson
 * @brief Controller task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Objects/MotorDriver.h"
#include "Tasks/ControllerTask.h"
#include "shares.h"
#include "Objects/APIHandler.h"

#define Solenoid_Pin 11

Controller::Controller(uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN, APIHandler api)
{
    server = api;
    state = 0; // Start state = 0
    uint16_t xStep = 0;
    uint16_t yStep = 0;
    uint8_t xLimPin = XLIM_PIN;
    uint8_t yLimPin = YLIM_PIN;
    uint8_t solenoidPin = SOLENOID_PIN;
    omega_Max = 500;     // steps per second
    pitch = 1 / 9;       // mm/deg
    step_size = .9;      // deg/step
    float origin_x = -5; // mm to origin of square x-direction
    float origin_y = -5; // mm to origin of square y-direction
    x_coordinate_from = 0;
    y_coordinate_from = 0;
    x_coordinate_to = 0;
    y_coordinate_to = 0;
}

/**
 * @brief Method called for multitasking. This controls the Mover FSM.
 */
void Controller::run() // Method for FSM
{
    switch (state)
    {
    case 0:
    {
        origin();
        if (scanBoard.get() == true)
        {
            state = 1; // Wait State
        }
        else
        {
            state = 9; // Scan Board State
        }
        break;
    }
    case 1:
    {
        waiting();
        if (beginMove.get() == true)
        {
            state = 2;
            beginMove.put(false);
        }
        break;
    }
    case 2:
    {
        x_coordinate_from = directionsQueue.get();
        y_coordinate_from = directionsQueue.get();
        x_coordinate_to = directionsQueue.get();
        y_coordinate_to = directionsQueue.get();
        movePiece(x_coordinate_from, y_coordinate_from);
        while (true)
        {
            if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                stopMotor1.put(false);
                stopMotor2.put(false);
                state = 3;
                break;
            }
            delay(10);
        }
        break;
    }
    case 3:
    {
        grabPiece();
        state = 4;
        break;
    }
    case 4:
    {
        squareOrigin();
        while (true)
        {
            if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                stopMotor1.put(false);
                stopMotor2.put(false);
                state = 5;
                break;
            }
            delay(10);
        }
        break;
    }
    case 5:
    {
        xGridMove(x_coordinate_to, x_coordinate_from);
        while (true)
        {
            if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                stopMotor1.put(false);
                stopMotor2.put(false);
                state = 6;
                break;
            }
            delay(10);
        }
        break;
    }
    case 6:
    {
        yGridMove(y_coordinate_to, y_coordinate_from);
        while (true)
        {
            if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                stopMotor1.put(false);
                stopMotor2.put(false);
                state = 7;
                break;
            }
            delay(10);
        }
        break;
    }
    case 7:
    {
        gridToCenter();
        while (true)
        {
            if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                stopMotor1.put(false);
                stopMotor2.put(false);
                state = 8;
                break;
            }
            delay(10);
        }
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
        scanBoard.put(true);
        while (true)
        {
            if (scanBoard.get() == false)
            {
                break;
            }
            delay(10);
        }
        state = 0;
        break;
    }
    }
}

void Controller::setState(uint8_t newState)
{
    state = newState;
}

void Controller::origin() // State 0
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
            dirMotor1.put(1);
            dirMotor2.put(-1);
            startMaxMotor1.put(true);
            startMaxMotor2.put(true);
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
            dirMotor1.put(1);
            dirMotor2.put(-1);
            startMaxMotor1.put(true);
            startMaxMotor2.put(true);
        }
    }
}
void Controller::waiting() // State 1
{
}

void Controller::movePiece(float move_From_x, float move_From_y) // State 2
{
    float Dx = move_From_x; // mm
    float Dy = move_From_y; // mm
    float omega_Lesser = -omega_Max * (2 * Dy / Dx - 1) / (2 * Dy / Dx + 1);
    uint16_t Num_Step_G = 2 * Dy / ((1 + ((Dy - 0.5 * Dx) / (Dy + 0.5 * Dx))) * step_size * pitch); // Num of steps for greater motor
    uint16_t Num_Step_L = 2 * Dy / (step_size * pitch) - Num_Step_G;
    aVel1.put(omega_Lesser);
    steps1.put(Num_Step_L);
    dirMotor2.put(1);
    steps2.put(Num_Step_G);
    startMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::grabPiece() // State 3
{
    digitalWrite(Solenoid_Pin, HIGH);
}

void Controller::squareOrigin() // State 4
{
    float omega_Lesser = omega_Max * (2 * origin_y / origin_x - 1) / (2 * origin_y / origin_x + 1);
    uint16_t Num_Step_G = 2 * origin_y / ((1 + ((origin_y - 0.5 * origin_x) / (origin_y + 0.5 * origin_x))) * step_size * pitch); // Num of steps for greater motor
    uint16_t Num_Step_L = 2 * origin_y / (step_size * pitch) - Num_Step_G;
    aVel1.put(omega_Lesser);
    steps1.put(Num_Step_L);
    dirMotor2.put(-1);
    steps2.put(Num_Step_G);
    startMotor1.put(true);
    startMaxMotor2.put(true);
}

void Controller::xGridMove(uint16_t x_to, uint16_t x_from) // State 5
{
    int16_t x_move = x_to - x_from; // mm
    uint16_t Num_Step = x_move / (pitch * step_size);
    int8_t direction = 0;
    if (x_move < 0)
    {
        direction = -1;
    }
    else
    {
        direction = 1;
    }
    steps1.put(Num_Step);
    steps2.put(Num_Step);
    dirMotor1.put(direction);
    dirMotor2.put(direction);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}

void Controller::yGridMove(uint16_t y_to, uint16_t y_from) // State 6
{
    int16_t y_move = y_to - y_from; // mm
    uint16_t Num_Step = y_move / (pitch * step_size);
    int8_t direction_1 = 0;
    int8_t direction_2 = 0;
    if (y_move < 0)
    {
        direction_1 = 1;
        direction_2 = -1;
    }
    else
    {
        direction_1 = -1;
        direction_2 = 1;
    }
    steps1.put(Num_Step);
    steps2.put(Num_Step);
    dirMotor1.put(direction_1);
    dirMotor2.put(direction_2);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::gridToCenter() // State 7
{
    float omega_Lesser = -omega_Max * (2 * origin_y / origin_x - 1) / (2 * origin_y / origin_x + 1);
    uint16_t Num_Step_G = 2 * origin_y / ((1 + ((origin_y - 0.5 * origin_x) / (origin_y + 0.5 * origin_x))) * step_size * pitch); // Num of steps for greater motor
    uint16_t Num_Step_L = 2 * origin_y / (step_size * pitch) - Num_Step_G;
    aVel1.put(omega_Lesser);
    steps1.put(Num_Step_L);
    dirMotor2.put(1);
    steps2.put(Num_Step_G);
    startMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::releasePiece() // State 8
{
    digitalWrite(Solenoid_Pin, LOW);
}

void Controller::runScan()
{
    scanBoard.put(true);
}