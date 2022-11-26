/**
 * @file Controller.h
 * @author Sam Hudson
 * @brief The controller manages communication between the
 * server and the mover task
 * @version 1.0
 * @date 2022-11-23
 */

#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "PubSubClient.h"


class Controller
{
private:

    char mqtt_server;
    char ssid;
    char password;

public:
    Controller(char *ssid, char *password, char *mqtt_server);
    void run();                                                                          // Method for FSM
};

#endif // _CONTROLLER_H
