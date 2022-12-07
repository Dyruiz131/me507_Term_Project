/**
 * @file APIHandler.h
 * @author Sam Hudson
 * @brief Handles the API communication between the server and the chess board
 * @version 1.0
 * @date 2022-11-27
 */
#include "APIHandler.h"

#include "WiFi.h"
#include "HTTPClient.h"
#include "Arduino_JSON.h"

/**
 * @brief Construct a new APIHandler::APIHandler object
 *
 * @param ssid WiFi SSID
 * @param password WiFi password
 * @param certificate SSL certificate (for HTTPS requests)
 */
APIHandler::APIHandler(const char *certificate)
{
    this->certificate = certificate; // Set the certificate
}

/**
 * @brief Default constructor necessary for dependency injection
 */
APIHandler::APIHandler()
{
}

/**
 * @brief Sends a GET request to a specified server
 *
 * @param URL The server URL to send the request to
 * @param cert The SSL certificate for the server
 * @return String response from the server
 */
String APIHandler::sendGET(const char *URL)
{
    WiFiClient client;
    HTTPClient http;

    // Start connection and send HTTP header to URL with certificate
    http.begin(URL, certificate);

    // Send HTTP GET request and save response code
    int httpResponseCode = http.GET();

    String res = "{}"; // For parsing the response

    // If successful query
    if (httpResponseCode > 0)
    {
        res = http.getString(); // Get the response payload
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode); // Print the error code
    }
    // Stop HTTP connection
    http.end();

    return res;
}
/**
 * @brief Sends a POST request to a specified server
 *
 * @param URL The server URL to send the request to
 * @param cert The SSL certificate for the server
 * @param req The request body to send
 * @return String response from the server
 */
String APIHandler::sendPOST(const char *URL, String req)
{
    WiFiClient client;
    HTTPClient http;

    // Start connection and send HTTP header to URL with certificate
    http.begin(URL, certificate);

    // Send HTTP POST request and save response code
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(req);

    String res = "{}"; // For parsing the response

    // If successful query
    if (httpResponseCode > 0)
    {
        res = http.getString(); // Get the response
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode); // Print the error code
    }
    // Stop HTTP connection
    http.end();

    return res;
}

/**
 * @brief Sends a move to the server
 *
 * @param from The square the piece is moving from
 * @param to The square the piece is moving to
 */
void APIHandler::sendMove(String from, String to)
{
    String req = "{\"from\":\"" + from + "\",\"to\":\"" + to + "\"}"; // Create request body in JSON format
    sendPOST("https://chessbotapi.onrender.com/move", req);
}

/**
 * @brief Gets the most recent move from the server
 *
 * @return String of the most recent move in the format "e2,e4"
 */
String APIHandler::getLatestMove()
{
    String res = sendGET("https://chessbotapi.onrender.com/lastMove");
    JSONVar jsonRes = JSON.parse(res);                   // Parse the response
    String returnMoveString = "";                        // String to return
    bool takePiece = jsonRes.hasOwnProperty("captured"); // Check if move is taking a piece
    String from = jsonRes["from"];
    String to = jsonRes["to"];

    // Set first value as flag for taking a piece
    if (takePiece) // Check if captured flag is returned from api
    {
        returnMoveString = "1"; // Taking a piece
    }
    else
    {
        returnMoveString = "0"; // Not taking a piece
    }

    returnMoveString = returnMoveString + from + to; // Create move string
    return returnMoveString;
}

/**
 * @brief Gets the current move status from the server
 *
 * @return boolean of whether the bot has completed its move
 */
bool APIHandler::getMoveStatus()
{
    String res = sendGET("https://chessbotapi.onrender.com/acceptMoves");
    JSONVar jsonRes = JSON.parse(res);    // Parse the response
    bool status = jsonRes["acceptMoves"]; // Get the status
    return status;
}

/**
 * @brief Sends the move status to the server
 *
 * @param status status of the move (if move is complete)
 */
void APIHandler::sendMoveStatus(bool moveIsComplete)
{
    String req = "{\"acceptMoves\":" + String(moveIsComplete) + "}"; // Create request body in JSON format
    sendPOST("https://chessbotapi.onrender.com/acceptMoves", req);
}

/**
 * @brief Checks if the game has just started
 *
 * @return true The game has just started
 * @return false The game has not just started
 */
bool APIHandler::isNewGame()
{

    String res = sendGET("https://chessbotapi.onrender.com/isNewGame");
    JSONVar jsonRes = JSON.parse(res);  // Parse the response
    bool status = jsonRes["isNewGame"]; // Get the new game status
    return status;
}

/**
 * @brief Creates a new game on the server (for if
 * a game is ongoing on startup)
 */
void APIHandler::newGame()
{
    String req = ""; // Empty body (not required for new game root)
    sendPOST("https://chessbotapi.onrender.com/new", req);
}