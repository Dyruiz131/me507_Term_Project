/**
 * @file Mover_Task.h
 * @author Sam Hudson
 * @brief Controller task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "shares.h"
#include "objects/APIHandler.h"
#include "objects/MotorDriver.h"
#include "tasks/ControllerTask.h"

Controller::Controller(uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN, APIHandler api)
{
    server = api;
    state = 0; // Start state = 0
    xStep = 0;
    yStep = 0;
    xLimPin = XLIM_PIN;
    yLimPin = YLIM_PIN;
    solenoidPin = SOLENOID_PIN;
    omegaMax = 500; // steps per second
    pitch = 1 / 9;  // mm/deg
    stepSize = .9;  // deg/step
    xOrigin = -5;   // mm to origin of square x-direction
    yOrigin = -5;   // mm to origin of square y-direction
    xCoordinateFrom = 0;
    yCoordinateFrom = 0;
    xCoordinateTo = 0;
    yCoordinateTo = 0;
    takePiece = 0;
    xPieceGraveyard = 0;
    yPieceGraveyard = 0;
    boardScanned = false;
}

/**
 * @brief Method called for multitasking.
 */
void Controller::run() // Method for FSM
{
    switch (state)
    {
    case 0:
    {
        origin();
        if (boardScanned) // If board has been scanned
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
        takePiece = directionsQueue.get();       // First val defines if piece needs taking first
        xCoordinateFrom = directionsQueue.get(); // Second val defines x coordinate of piece to move
        yCoordinateFrom = directionsQueue.get(); // Third val defines y coordinate of piece to move
        xCoordinateTo = directionsQueue.get();   // Fourth val defines x coordinate of piece to move to
        yCoordinateTo = directionsQueue.get();   // Fifth val defines y coordinate of piece to move to
        if (takePiece == 1)
        { // If piece needs taking
            state = 10;
            break;
        }
        else
        {
            movePiece(xCoordinateFrom, yCoordinateFrom); // Move to piece
            waitMotorStop();
            state = 3;
            break;
        }
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
        waitMotorStop();
        state = 5;
        break;
    }
    case 5:
    {
        if (takePiece)
        {
            xGridMove(xPieceGraveyard, xCoordinateTo);
            waitMotorStop();
            state = 6;
            break;
        }
        else
        {

            xGridMove(xCoordinateTo, xCoordinateFrom);
            waitMotorStop();
            state = 6;
            break;
        }
    }
    case 6:
    {
        if (takePiece)
        {
            yGridMove(yPieceGraveyard, yCoordinateTo);
            waitMotorStop();
            state = 7;
            break;
        }
        else
        {

            yGridMove(yCoordinateTo, yCoordinateFrom);
            waitMotorStop();
            state = 7;
            break;
        }
    }
    case 7:
    {
        gridToCenter();
        waitMotorStop();
        state = 8;
        break;
    }
    case 8:
    {
        releasePiece();
        if (takePiece == 1)
        {
            boardScanned = true;
        }
        else
        {
            boardScanned = false;
        }
        takePiece = 0;
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
    case 10:
    {
        movePiece(xCoordinateTo, yCoordinateTo);
        while (true)
        {
            if (stopMotor1.get() && stopMotor2.get())
            {
                stopMotor1.put(false);
                stopMotor2.put(false);
                break;
            }
            delay(10);
        }
        state = 3;
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
            steps1.put(1);
            steps2.put(1);
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
            steps1.put(1);
            steps2.put(1);
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

void Controller::movePiece(float moveFromX, float moveFromY) // State 2
{
    float Dx = moveFromX; // mm
    float Dy = moveFromY; // mm
    float omegaLesser = -omegaMax * (2 * Dy / Dx - 1) / (2 * Dy / Dx + 1);
    uint16_t numStepsG = 2 * Dy / ((1 + ((Dy - 0.5 * Dx) / (Dy + 0.5 * Dx))) * stepSize * pitch); // Num of steps for greater motor
    uint16_t numStepsL = 2 * Dy / (stepSize * pitch) - numStepsG;
    aVel1.put(omegaLesser);
    steps1.put(numStepsL);
    dirMotor2.put(1);
    steps2.put(numStepsG);
    startMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::grabPiece() // State 3
{
    digitalWrite(solenoidPin, HIGH);
}

void Controller::squareOrigin() // State 4
{
    float omegaLesser = omegaMax * (2 * yOrigin / xOrigin - 1) / (2 * yOrigin / xOrigin + 1);
    uint16_t numStepsG = 2 * yOrigin / ((1 + ((yOrigin - 0.5 * xOrigin) / (yOrigin + 0.5 * xOrigin))) * stepSize * pitch); // Num of steps for greater motor
    uint16_t numStepsL = 2 * yOrigin / (stepSize * pitch) - numStepsG;
    aVel1.put(omegaLesser);
    steps1.put(numStepsL);
    dirMotor2.put(-1);
    steps2.put(numStepsG);
    startMotor1.put(true);
    startMaxMotor2.put(true);
}

void Controller::xGridMove(uint16_t xTo, uint16_t xFrom) // State 5
{
    int16_t xMove = xTo - xFrom; // mm
    uint16_t numSteps = xMove / (pitch * stepSize);
    int8_t direction = 0;
    if (xMove < 0)
    {
        direction = -1;
    }
    else
    {
        direction = 1;
    }
    steps1.put(numSteps);
    steps2.put(numSteps);
    dirMotor1.put(direction);
    dirMotor2.put(direction);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}

void Controller::yGridMove(uint16_t yTo, uint16_t yFrom) // State 6
{
    int16_t yMove = yTo - yFrom; // mm
    uint16_t numSteps = yMove / (pitch * stepSize);
    int8_t direction1 = 0;
    int8_t direction2 = 0;
    if (yMove < 0)
    {
        direction1 = 1;
        direction2 = -1;
    }
    else
    {
        direction1 = -1;
        direction2 = 1;
    }
    steps1.put(numSteps);
    steps2.put(numSteps);
    dirMotor1.put(direction1);
    dirMotor2.put(direction2);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::gridToCenter() // State 7
{
    float omegaLesser = -omegaMax * (2 * yOrigin / xOrigin - 1) / (2 * yOrigin / xOrigin + 1);
    uint16_t numStepsG = 2 * yOrigin / ((1 + ((yOrigin - 0.5 * xOrigin) / (yOrigin + 0.5 * xOrigin))) * stepSize * pitch); // Num of steps for greater motor
    uint16_t numStepsL = 2 * yOrigin / (stepSize * pitch) - numStepsG;
    aVel1.put(omegaLesser);
    steps1.put(numStepsL);
    dirMotor2.put(1);
    steps2.put(numStepsG);
    startMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::releasePiece() // State 8
{
    digitalWrite(solenoidPin, LOW);
}

void Controller::waitMotorStop()
{
    while (!(stopMotor1.get() && stopMotor2.get()))
    {
        delay(10);
    }
    stopMotor1.put(false);
    stopMotor2.put(false);
}