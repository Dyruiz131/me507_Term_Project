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

    gridCoordinates[0] = 0.0;
    for (int i = 0; i < 8; i++)
    {                             // Create grid coordinates
        gridCoordinates[i] += 60; // 60mm between each piece
    }

    state = 0;     // Start state = 0
    lastMove = ""; // Store last move for comparison
    newMove = "";  // Store new move for comparison
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
            uint8_t takePiece = newMove.substring(0).toInt(); // If piece needs taking
            char fromCol = newMove.charAt(1);                 // From x
            uint8_t fromRow = newMove.substring(2).toInt();   // From y
            char toCol = newMove.charAt(3);                   // To x
            uint8_t toRow = newMove.substring(4).toInt();     // To y

            directionsQueue.put(takePiece);             // Take Piece Flag
            directionsQueue.put(toCoordinate(fromCol)); // From x
            directionsQueue.put(toCoordinate(fromRow)); // From y
            directionsQueue.put(toCoordinate(toCol));   // To x
            directionsQueue.put(toCoordinate(toRow));   // To y
            beginMove.put(true);                        // Begin move
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
    switch (col)
    {
    case 'a':
        return gridCoordinates[0];
    case 'b':
        return gridCoordinates[1];
    case 'c':
        return gridCoordinates[2];
    case 'd':
        return gridCoordinates[3];
    case 'e':
        return gridCoordinates[4];
    case 'f':
        return gridCoordinates[5];
    case 'g':
        return gridCoordinates[6];
    case 'h':
        return gridCoordinates[7];
    default:
        return 0;
    }
}

float FetchMove::toCoordinate(uint8_t row)
{
    return gridCoordinates[row - 1];
}