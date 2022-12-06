/**
 * @file FetchMoveTask.cpp
 * @author Sam Hudson
 * @brief Provides the FSM that fetches moves
 * from the server and converts them to coordinates
 * (to be placed in the move queue)
 * @version 1.0
 * @date 2022-11-30
 */

#include <Arduino.h>
#include "tasks/FetchMoveTask.h"
#include "shares.h"

/**
 * @brief Construct a new FetchMoveTask object
 *
 * @param api The API handler object that will communicate with the server
 */
FetchMove::FetchMove(APIHandler api)
{
    this->api = api;
    xOffset = 10; // x offset from origin to game board origin
    yOffset = 10; // y offset from origin to game board origin

    // Set grid locations manually (in mm from origin)
    gridCoordinates[0] = 30.0;
    gridCoordinates[1] = 90.0;
    gridCoordinates[2] = 150.0;
    gridCoordinates[3] = 210.0;
    gridCoordinates[4] = 270.0;
    gridCoordinates[5] = 330.0;
    gridCoordinates[6] = 390.0;
    gridCoordinates[7] = 450.0;

    state = 0;      // Start state = 0
    lastMove = "0"; // Store last move for comparison, initialise with "0"
    newMove = "0";  // Store new move for comparison, initialise with "0"
}

/**
 * @brief Default constructor necessary for dependency injection
 */
FetchMove::FetchMove()
{
}

/**
 * @brief FSM that fetches moves from the server and converts them to coordinates
 *
 */
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
            lastMove = "0"; // Reset last move since game is new
            break;
        }
        case 2: // Check for new moves
        {
            newMove = api.getLatestMove(); // Get latest move
            if (newMove != lastMove)
            { // If there is a new move
                state = 3;
                Serial.println("lastMove:");
                Serial.println(lastMove);
                Serial.println("newMove:");
                Serial.println(newMove);
            }
            lastMove = newMove; // Set last move for future comparison
            break;
        }
        case 3: // Calculate and send coordinates to queue
        {
            // Serial.println("Fetch state 3");
            uint8_t takePiece = newMove.substring(0).toInt(); // If piece needs taking
            char fromCol = newMove.charAt(1);                 // From x
            uint8_t fromRow = newMove.substring(2).toInt();   // From y
            char toCol = newMove.charAt(3);                   // To x
            uint8_t toRow = newMove.substring(4).toInt();     // To y

            Serial.println(takePiece);
            Serial.println(fromCol);
            Serial.println(fromRow);
            Serial.println(toCol);
            Serial.println(toRow);

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
            state = 4;
            break;
        }
        case 4: // Wait for move to complete before checking for new moves
        {
            if (moveComplete.get()) // Wait for move to complete
            {
                api.sendMoveStatus(true); // Set move complete status to true on server
                state = 2;
            }
            break;
        }
        }
    }
}

/**
 * @brief Converts the chessboard coordinate of a column to a coordinate
 *
 * @param col the chessboard coordinate of the column
 * @return float Board coordinate (in mm)
 */
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

/**
 * @brief Converts the chessboard coordinate of a row to a coordinate
 *
 * @param col the chessboard coordinate of the row
 * @return float Board coordinate (in mm)
 */
float FetchMove::toCoordinate(uint8_t row)
{
    return gridCoordinates[row - 1];
}