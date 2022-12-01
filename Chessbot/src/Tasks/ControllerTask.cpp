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
#include "kinematics.h"

Controller::Controller(uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN, uint8_t SENSOR_PIN)
{
    state = 0; // Start state = 0
    xStep = 0;
    yStep = 0;
    xLimPin = XLIM_PIN;
    yLimPin = YLIM_PIN;
    solenoidPin = SOLENOID_PIN;
    sensorPin = SENSOR_PIN;
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
    xPieceGraveyard = 520;
    yPieceGraveyard = 522.5;
    sensorOffset = 18.25;
    Kinematics kinematics;
    beginMove.put(false);
    uint8_t count = 0;
}

/**
 * @brief Method called for multitasking.
 */
void Controller::run() // Method for FSM
{
    switch (state)
    {
        case 0: // Calibration
        {
            Serial.println("State 0");
            origin();
            state = 1;
            break;
        }
        case 1: // Check for a move request
        {
            Serial.println("State 1:");
            grabPiece(); // Release solenoid to stop meltdown
            if (beginMove.get() == true)
            {
                state = 2;
                beginMove.put(false); // Reset the flag
            }
            break;
        }
        case 2: // Move to piece
        {
            Serial.println("State 2:");
            takePiece = directionsQueue.get();       // First val defines if piece needs taking first
            xCoordinateFrom = directionsQueue.get(); // Second val defines x coordinate of piece to move
            yCoordinateFrom = directionsQueue.get(); // Third val defines y coordinate of piece to move
            xCoordinateTo = directionsQueue.get();   // Fourth val defines x coordinate of piece to move to
            yCoordinateTo = directionsQueue.get();   // Fifth val defines y coordinate of piece to move to
            break;
        }
        case 3:
        {
            releasePiece();
            if (takePiece == 1)
            { // If piece needs taking
                state = 18;
            }
            else
            {
                movePiece(xCoordinateFrom - sensorOffset, yCoordinateFrom); // Move to piece
                state = 4;
            }
            break;
        }

        case 4:
        {
            if((stopMotor1.get() == true, stopMotor2.get() == true))
            {
                state = 5;
                count = 0;
            }
            break;
        }

        case 5: //detect piece
        {
            Serial.println("State 4:");
            if (detectPiece())
            {
                count += 1;
            }
            if (count > 10)
            {
                state = 6;
            }
            break;
        }

        case 6:

        {
            movePiece(sensorOffset, 0); // Move to piece
            state = 7;
            break;
        }
        case 7:
        {
            if((stopMotor1.get() == true && stopMotor2.get() == true))
            {

                state = 8;
            }
            break;
        }
        case 8:
        {
            grabPiece();
            state = 9;
            break;
        }

        case 9: // Move to grid before moving along gridlines
        {
            Serial.println("State 4:");
            centerToGrid();
            state = 10;
            break;
        }

        case 10:
        {
            if((stopMotor1.get() == true && stopMotor2.get() == true))
            {

                state = 11;
              
            }
            break;
        }
    
        case 11: // Move along x gridline
        {
            if (takePiece)
            {
                xGridMove(xPieceGraveyard, xCoordinateTo);
                state = 12;
            }
            else
            {

                xGridMove(xCoordinateTo, xCoordinateFrom);
                state = 12;
            }
            break;
        }

        case 12:
        {
            if((stopMotor1.get() == true && stopMotor2.get() == true))
            {

                state = 13;
              
            }
            break;
        }
    
        case 13: // Move along y gridline
        {
            if (takePiece)
            {
                yGridMove(yPieceGraveyard, yCoordinateTo);
                state = 14;
            }
            else
            {

                yGridMove(yCoordinateTo, yCoordinateFrom);
                state = 14;
            }
            break;
        }

        case 14:
        {
            if((stopMotor1.get() == true && stopMotor2.get() == true))
            {

                state = 15;
              
            }
            break;
        }

        case 15:
        {
            if (takePiece)
            {
                gridToGraveyard();
            }
            else
            {
                gridToCenter();
            }

            state = 16;
            break;
        }

        case 16:
        {
            if((stopMotor1.get() == true && stopMotor2.get() == true))
            {

                state = 17;
              
            }
            break;
        }

        case 17:
        {
            releasePiece();
            if (takePiece == 0)
            {
                moveComplete.put(true); // Signal that move is complete
            }
            takePiece = 0;
            state = 0;
            break;
        }
        case 18: // Move to piece that needs taking
        {
            movePiece(xCoordinateTo - sensorOffset, yCoordinateTo);
        
            state = 19;
            break;
        }
        case 19:
        {
            if((stopMotor1.get() == true && stopMotor2.get() == true))
            {

                state = 3;
              
            }
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
    if (!digitalRead(xLimPin)) // If x lim switch is hit, stop moving.
    {
        xStep = 0; // Set x position to 0
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
    /* Check y axis */
    if (!digitalRead(yLimPin)) // If y lim switch is hit, stop moving.
    {
        yStep = 0; // Set y position to 0
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

void Controller::movePiece(float moveFromX, float moveFromY) // State 2
{
    float Dx = moveFromX; // mm
    float Dy = moveFromY; // mm
    int16_t velocityMotor1 = kinematics.coordsToVelocityMotor1(Dx, Dy);
    int16_t velocityMotor2 = kinematics.coordsToVelocityMotor2(Dx, Dy);
    uint16_t stepsMotor1 = kinematics.coordsToStepsMotor1(Dx, Dy);
    uint16_t stepsMotor2 = kinematics.coordsToStepsMotor2(Dx, Dy);

    steps1.put(stepsMotor1);
    steps2.put(stepsMotor2);
    aVel1.put(velocityMotor1);
    aVel2.put(velocityMotor2);
    startMotor1.put(true);
    startMotor2.put(true);
}
void Controller::grabPiece() // State 3
{
    digitalWrite(solenoidPin, LOW);
}

void Controller::centerToGrid() // State 4
{
    movePiece(-30, -30);
}

void Controller::gridToCenter() // State 7
{
    movePiece(30, 30);
}

void Controller::gridToGraveyard() // State 4
{
    movePiece(10, 30);
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

void Controller::releasePiece() // State 8
{
    digitalWrite(solenoidPin, HIGH);
}


bool Controller::detectPiece()
{
    return digitalRead(sensorPin) == LOW;
}