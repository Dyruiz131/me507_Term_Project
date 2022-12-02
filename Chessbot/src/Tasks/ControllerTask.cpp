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
    xOrigin = -5;   // mm to origin of square x-direction
    yOrigin = -5;   // mm to origin of square y-direction
    stepLength = .1;
    xCoordinateFrom = 0;
    yCoordinateFrom = 0;
    xCoordinateTo = 0;
    yCoordinateTo = 0;
    takePiece = 0;
    moveTake = 0;
    xPieceGraveyard = 520;
    yPieceGraveyard = 522.5 - 180;
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
    case 0: // Calibrate x axis
    {
        releasePiece();
        origin_x();
        startLimitx.put(true);
        state = 20;
        break;
    }
    case 20: // Check if motors have stopped
    {
        if ((stopMotor1.get() == true, stopMotor2.get() == true))
        {
            state = 21;
        }
        break;
    }
    case 21: // Calibrate y axis
    {
        grabPiece();
        origin_y();
        startLimity.put(true);
        state = 22;
        break;
    }
    case 22: // Check if motors have stopped
    {
        if ((stopMotor1.get() == true, stopMotor2.get() == true))
        {
            state = 1;
            // Release solenoid to stop meltdown
        }
        break;
    }

    case 1: // Check for a move request (waiting state)
    {
        grabPiece();
        if (moveTake == 1) // If a piece needs taking
        {
            state = 3;
            moveTake = 0;
        }
        else if (beginMove.get() == true) // If a move is requested
        {
            state = 2;
            beginMove.put(false);    // Reset the flag
            moveComplete.put(false); // Tell API that move is not complete
        }
        break;
    }
    case 2: // Get new move from FetchMoveTask
    {
        moveComplete.put(false);
        takePiece = directionsQueue.get();       // First val defines if piece needs taking first
        xCoordinateFrom = directionsQueue.get(); // Second val defines x coordinate of piece to move
        yCoordinateFrom = directionsQueue.get(); // Third val defines y coordinate of piece to move
        xCoordinateTo = directionsQueue.get();   // Fourth val defines x coordinate of piece to move to
        yCoordinateTo = directionsQueue.get();   // Fifth val defines y coordinate of piece to move to
        state = 3;
        // Serial.println(xCoordinateFrom);
        // Serial.println(xCoordinateTo);
        // Serial.println(yCoordinateFrom);
        // Serial.println(yCoordinateTo);
        break;
    }
    case 3: // Stop motors, move to piece along x
    {
        stopMotor1.put(false);
        stopMotor2.put(false);
        if (takePiece == 1) // If a piece needs taking
        {
            state = 18; // Move to piece to take
        }
        else
        {
            movePiecex(xCoordinateFrom - sensorOffset, yCoordinateFrom); // Move to piece
            state = 23;
        }
        break;
    }
    case 23: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true, stopMotor2.get() == true))
        {
            state = 24;
            count = 0;
        }
        break;
    }

    case 24: // Stop motors, move to piece along y
    {
        stopMotor1.put(false);
        stopMotor2.put(false);
        movePiecey(xCoordinateFrom - sensorOffset, yCoordinateFrom); // Move to piece
        state = 4;

        break;
    }

    case 4: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true, stopMotor2.get() == true))
        {
            state = 5;
            count = 0;
        }
        break;
    }

    case 5: // Check if sensor is under piece
    {
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

    case 6: // Move actuator under piece
    {
        movePiecex(sensorOffset, 0); // Move to piece
        state = 7;
        break;
    }
    case 7: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 8;
        }
        break;
    }
    case 8: // Grab piece
    {
        grabPiece();
        state = 9;
        break;
    }

    case 9: // Move to grid before moving along gridlines (along x)
    {
        centerToGridx();
        state = 10;
        break;
    }

    case 10: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 25;
        }
        break;
    }

    case 25: // Move to grid before moving along gridlines (along y)
    {
        centerToGridy();
        state = 26;
        break;
    }

    case 26: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 11;
        }
        break;
    }

    case 11: // Move along x gridline
    {
        if (takePiece == 1) // If piece needs taking
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

    case 12: // Wait for motor to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 13;
        }
        break;
    }

    case 13: // Move along y gridline
    {
        if (takePiece == 1) // If piece needs taking
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

    case 14: // Wait for motor to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 15;
        }
        break;
    }

    case 15: // Move piece along x for final position
    {
        if (takePiece) // If piece needs taking
        {
            gridToGraveyardx(); // Take piece to graveyard along x
        }
        else
        {
            gridToCenterx(); // Move piece to x center of sqaure from grid
        }

        state = 16;
        break;
    }

    case 16: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 27;
        }
        break;
    }

    case 27: // Move piece along y for final position
    {
        if (takePiece) // If piece needs taking
        {
            gridToGraveyardy(); // Take piece to graveyard along y
        }
        else
        {
            gridToCentery(); // Move piece to y center of sqaure from grid
        }

        state = 28;
        break;
    }

    case 28: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 17;
        }
        break;
    }

    case 17: // Release piece
    {
        releasePiece();
        if (takePiece == 0) // Check if piece did not need taking
        {
            moveComplete.put(true); // Tell API that move is complete
        }
        if (takePiece == 1) // If piece needs taking
        {
            moveTake = 1; // Set moveTake to 1 to signal that piece needs taking
        }
        takePiece = 0; // Reset takePiece flag
        state = 0;
        break;
    }
    case 18: // Move to piece that needs taking along x grid
    {
        movePiecex(xCoordinateTo - sensorOffset, yCoordinateTo);

        state = 19;
        break;
    }
    case 19: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 29;
        }
        break;
    }

    case 29: // Move to piece that needs taking along y grid
    {
        movePiecey(xCoordinateTo - sensorOffset, yCoordinateTo);

        state = 30;
        break;
    }

    case 30: // Wait for motors to stop
    {
        if ((stopMotor1.get() == true && stopMotor2.get() == true))
        {

            state = 5; // Take piece
        }
        break;
    }
    }
}

void Controller::setState(uint8_t newState)
{
    state = newState;
}

void Controller::origin_x() // State 0
{
    /* Check x axis */
    // Move left 1 step
    steps1.put(10000);
    steps2.put(10000);
    dirMotor1.put(1);
    dirMotor2.put(1);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::origin_y() // State 0
{

    // Move down 1 step
    steps1.put(10000);
    steps2.put(10000);
    dirMotor1.put(1);
    dirMotor2.put(-1);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}

void Controller::movePiecex(float moveFromX, float moveFromY) // State 2
{
    float Dx = moveFromX; // mm
    float Dy = moveFromY; // mm
    int16_t velocityMotor1 = kinematics.coordsToVelocityMotor1(Dx, 0);
    int16_t velocityMotor2 = kinematics.coordsToVelocityMotor2(Dx, 0);
    uint16_t stepsMotor1 = kinematics.coordsToStepsMotor1(Dx, 0);
    uint16_t stepsMotor2 = kinematics.coordsToStepsMotor2(Dx, 0);

    Serial.println(Dx);
    Serial.println(Dy);
    Serial.println(velocityMotor1);
    Serial.println(velocityMotor2);
    Serial.println(stepsMotor1);
    Serial.println(stepsMotor2);

    steps1.put(stepsMotor1);
    steps2.put(stepsMotor2);
    if (velocityMotor1 > 0)
    {
        dirMotor1.put(1);
    }
    else
    {
        dirMotor1.put(-1);
    }

    if (velocityMotor2 > 0)
    {
        dirMotor2.put(1);
    }
    else
    {
        dirMotor2.put(-1);
    }

    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}

void Controller::movePiecey(float moveFromX, float moveFromY) // State 2
{
    float Dx = moveFromX; // mm
    float Dy = moveFromY; // mm
    int16_t velocityMotor1 = kinematics.coordsToVelocityMotor1(0, Dy);
    int16_t velocityMotor2 = kinematics.coordsToVelocityMotor2(0, Dy);
    uint16_t stepsMotor1 = kinematics.coordsToStepsMotor1(0, Dy);
    uint16_t stepsMotor2 = kinematics.coordsToStepsMotor2(0, Dy);

    Serial.println(Dx);
    Serial.println(Dy);
    Serial.println(velocityMotor1);
    Serial.println(velocityMotor2);
    Serial.println(stepsMotor1);
    Serial.println(stepsMotor2);

    steps1.put(stepsMotor1);
    steps2.put(stepsMotor2);

    if (velocityMotor1 > 0)
    {
        dirMotor1.put(1);
    }
    else
    {
        dirMotor1.put(-1);
    }

    if (velocityMotor2 > 0)
    {
        dirMotor2.put(1);
    }
    else
    {
        dirMotor2.put(-1);
    }

    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
}
void Controller::grabPiece() // State 3
{
    digitalWrite(solenoidPin, LOW);
}

void Controller::centerToGridx() // State 4
{
    movePiecex(-30, 0);
}

void Controller::centerToGridy() // State 4
{
    movePiecey(0, -30);
}

void Controller::gridToCenterx() // State 7
{
    movePiecex(30, 0);
}

void Controller::gridToCentery() // State 7
{
    movePiecey(0, 30);
}

void Controller::gridToGraveyardx() // State 4
{
    movePiecex(10, 0);
}

void Controller::gridToGraveyardy() // State 4
{
    movePiecey(0, 30);
}

void Controller::xGridMove(uint16_t xTo, uint16_t xFrom) // State 5
{
    int16_t xMove = xTo - xFrom; // mm
    uint16_t numSteps = abs(xMove / (stepLength));
    int8_t direction = 0;
    if (xMove == 0)
    {
        stopMotor1.put(true);
        stopMotor2.put(true);
    }
    else
    {
        if (xMove < 0)
        {
            direction = 1;
        }
        else
        {
            direction = -1;
        }
        steps1.put(numSteps);
        steps2.put(numSteps);
        dirMotor1.put(direction);
        dirMotor2.put(direction);
        startMaxMotor1.put(true);
        startMaxMotor2.put(true);
    }
}

void Controller::yGridMove(uint16_t yTo, uint16_t yFrom) // State 6
{
    int16_t yMove = yTo - yFrom; // mm
    uint16_t numSteps = abs(yMove / (stepLength));
    int8_t direction1 = 0;
    int8_t direction2 = 0;
    if (yMove == 0)
    {
        stopMotor1.put(true);
        stopMotor2.put(true);
    }
    else
    {
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
}

void Controller::releasePiece() // State 8
{
    digitalWrite(solenoidPin, HIGH);
}

bool Controller::detectPiece()
{
    return digitalRead(sensorPin) == LOW;
}