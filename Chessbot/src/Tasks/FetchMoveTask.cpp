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
    xOffset = 10;   // x offset from origin to game board origin
    yOffset = 12.5; // y offset from origin to game board origin
    gridCoordinates[0] = 30.0;
    for (int i = 1; i < 8; i++)
    {                                                     // Create grid coordinates
        gridCoordinates[i] = gridCoordinates[i - 1] + 60; // 60mm between each piece
    }

    state = 0;     // Start state = 0
    lastMove = "0"; // Store last move for comparison
    newMove = "0";  // Store new move for comparison
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
            Serial.println("Fetch state 2");
            newMove = api.getLatestMove(); // Get latest move
            Serial.print(
                "new move:");
            Serial.println(newMove);
            if (newMove != lastMove) // If a new move is fetched
            {
                lastMove = newMove; // Set future evaluations
                state = 3;
            }
            break;
        }
        case 3: // Calculate and send coordinates to queue
        {
            Serial.println("Fetch state 3");
            uint8_t takePiece = newMove.substring(0).toInt(); // If piece needs taking
            char fromCol = newMove.charAt(1);                 // From x
            uint8_t fromRow = newMove.substring(2).toInt();   // From y
            char toCol = newMove.charAt(3);                   // To x
            uint8_t toRow = newMove.substring(4).toInt();     // To y

            float xCoordinateFrom = toCoordinate(fromCol) + xOffset; // Convert to coordinates
            float yCoordinateFrom = toCoordinate(fromRow) + yOffset; // Convert to coordinates
            float xCoordTo = toCoordinate(toCol) + xOffset;          // Convert to coordinates
            float yCoordinateTo = toCoordinate(toRow) + yOffset;     // Convert to coordinates

            directionsQueue.put(takePiece);       // Take Piece Flag
            directionsQueue.put(xCoordinateFrom); // From x
            directionsQueue.put(yCoordinateFrom); // From y
            directionsQueue.put(xCoordTo);        // To x
            directionsQueue.put(yCoordinateTo);   // To y
            beginMove.put(true);                  // Begin move
            state = 2;
            break;
        }
        case 4:
        {
            if (moveComplete.get())
            {
                moveDone(); // Update server state
                state = 2;
            }
            break;
        }
        }
    }
}

void FetchMove::moveDone()
{
    api.sendMoveStatus(true);
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