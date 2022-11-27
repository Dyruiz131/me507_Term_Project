/**
 * @file APIHandler.h
 * @author Sam Hudson
 * @brief Handles the API communication between the server and the chess board
 * @version 1.0
 * @date 2022-11-27
 */

#ifndef _APIHANDLER_H_
#define _APIHANDLER_H_

#include <Arduino.h>
#include <WiFi.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "PubSubClient.h"

class APIHandler
{
private:
    const char *ssid;
    const char *password;
    const char *certificate;

public:
    APIHandler(const char *wifiSSID, const char *pass);
    void sendMove(String from, String to);
    String getFen();
    char getTurn();
    String getLastMove();
    boolean getMoveStatus();
    void sendMoveStatus(boolean status);
};

#endif // _APIHANDLER_H_
