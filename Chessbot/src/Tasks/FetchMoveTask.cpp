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
FetchMove::FetchMove(APIHandler api)
{
    this->api = api;
    state = 0;
    lastMove = "";
    newMove = "";
}

/**
 * @brief Default constructor necessary for dependency injection
 */
FetchMove::FetchMove()
{
}

void FetchMove::run()
{
    while (true)
    {
        switch (state)
        {
        case 0:
        {
            if (api.isNewGame())
            {
                state = 2; // Move to state 2
            }
            else
            {
                state = 1; // Move to state 1
            }
            break;
        }
        case 1:
        {
            api.newGame(); // Send new game request
            state = 0;
            break;
        }
        case 2:
        {
            newMove = api.getLatestMove(); // Get latest move
            if (newMove != lastMove)
            {
                lastMove = newMove; // Set future evaluations
                state = 3;
            }
            break;
        }
        case 3: // Calculate and send coordinates to queue
        {
            uint8_t takePiece = newMove.substring(0).toInt();
            char fromCol = newMove.charAt(1);               // From x
            uint8_t fromRow = newMove.substring(2).toInt(); // From y
            char toCol = newMove.charAt(3);
            uint8_t toRow = newMove.substring(4).toInt();

            directionsQueue.put(takePiece);           // Take Piece Flag
            directionsQueue.put(colToCoord(fromCol)); // From x
            directionsQueue.put(rowToCoord(fromRow)); // From y
            directionsQueue.put(colToCoord(toCol));   // To x
            directionsQueue.put(rowToCoord(toRow));   // To y

            state = 2;
            break;
        }
        }
    }
}

void FetchMove::moveDone()
{
    api.sendMoveStatus(true);
}

void FetchMove::moveFailed()
{
    api.sendMoveStatus(false);
}

float FetchMove::toCoordinate(char col)
{
    float gridLocations[8] = {30, 90, 150, 210, 270, 330, 390, 450};
    switch (col)
    {
    case 'a':
        return gridLocations[0];
    case 'b':
        return gridLocations[1];
    case 'c':
        return gridLocations[2];
    case 'd':
        return gridLocations[3];
    case 'e':
        return gridLocations[4];
    case 'f':
        return gridLocations[5];
    case 'g':
        return gridLocations[6];
    case 'h':
        return gridLocations[7];
    default:
        return 0;
    }
}

float FetchMove::toCoordinate(uint8_t row)
{
    float gridLocations[8] = {30, 90, 150, 210, 270, 330, 390, 450};
    return gridLocations[row - 1];
}