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
    this->certificate = certificate;
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
String sendGET(const char *URL, const char *cert)
{
    WiFiClient client;
    HTTPClient http;

    // Start connection and send HTTP header to URL with certificate
    http.begin(URL, cert);

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    String res = "{}";

    // If successful query
    if (httpResponseCode > 0)
    {
        res = http.getString(); // Get the response payload
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
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
String sendPOST(const char *URL, const char *cert, String req)
{
    WiFiClient client;
    HTTPClient http;

    // Start connection and send HTTP header to URL with certificate
    http.begin(URL, cert);

    // Send HTTP POST request
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(req);

    String res = "{}";

    // If successful query
    if (httpResponseCode > 0)
    {
        res = http.getString(); // Get the response
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
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
    String req = "{\"from\":\"" + from + "\",\"to\":\"" + to + "\"}";
    sendPOST("https://chessbotapi.onrender.com/move", certificate, req);
}

/**
 * @brief Gets the most recent move from the server
 *
 * @return String of the most recent move in the format "e2,e4"
 */
String APIHandler::getLatestMove()
{
    String res = sendGET("https://chessbotapi.onrender.com/lastMove", certificate);
    JSONVar jsonRes = JSON.parse(res);
    String lastMove;
    String takeMove = "0";
    if (JSON.typeof(jsonRes) == "undefined")
    {
        // Serial.println("Parsing JSON failed.");
        lastMove = takeMove;
    }
    else
    {
        if (jsonRes["lastMove.captured"])
        {
            takeMove = "1";
        }
        String from = jsonRes["from"];
        String to = jsonRes["to"];
        lastMove = takeMove + from + to;
    }
    return lastMove;
}

/**
 * @brief Gets the current move status from the server
 *
 * @return boolean of whether the bot has completed its move
 */
bool APIHandler::getMoveStatus()
{
    String res = sendGET("https://chessbotapi.onrender.com/acceptMoves", certificate);
    JSONVar jsonRes = JSON.parse(res);
    bool status = jsonRes["acceptMoves"];
    return status;
}

/**
 * @brief Sends the move status to the server
 *
 * @param status status of the move (if move is complete)
 */
void APIHandler::sendMoveStatus(bool status)
{
    String req = "{\"acceptMoves\":" + String(status) + "}";
    sendPOST("https://chessbotapi.onrender.com/acceptMoves", certificate, req);
}

/**
 * @brief Checks if the game has just started
 *
 * @return true The game has just started
 * @return false The game has not just started
 */
bool APIHandler::isNewGame()
{

    String res = sendGET("https://chessbotapi.onrender.com/isNewGame", certificate);
    JSONVar jsonRes = JSON.parse(res);
    bool status = jsonRes["isNewGame"];
    Serial.println(status);
    return status;
}

/**
 * @brief Creates a new game on the server (for if
 * a game is ongoing on startup)
 */
void APIHandler::newGame()
{
    String req = "";
    sendPOST("https://chessbotapi.onrender.com/new", certificate, req);
}