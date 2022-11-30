/**
 * @file APIHandler.h
 * @author Sam Hudson
 * @brief Handles the API communication between the server and the chess board
 * @version 1.0
 * @date 2022-11-27
 */

#ifndef _API_HANDLER_H_
#define _API_HANDLER_H_

#include <Arduino.h>
#include <WiFi.h>
#include "HTTPClient.h"
#include "Arduino_JSON.h"
#include "taskqueue.h"
#include "taskshare.h"

class APIHandler
{
private:
    const char *ssid;
    const char *password;
    const char *certificate;

public:
    APIHandler(const char *ssid, const char *password, const char *certificate);
    APIHandler(); // Default constructor
    void sendMove(String from, String to);
    String getFen();
    char getTurn();
    String getLastMove();
    bool getMoveStatus();
    void sendMoveStatus(bool status);
};

#endif // _API_HANDLER_H_
