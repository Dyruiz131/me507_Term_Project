/**
 * @file Controller.h
 * @author Sam Hudson
 * @brief The controller manages communication between the
 * server and the mover task
 * @version 1.0
 * @date 2022-11-23
 */
#include "Controller.h"
#include "wifipass.cpp" //Store wifi password in this file (not included in git)



Controller::Controller(char *wifissid, char *pass, char *mqtt_addr)
{   
    mqtt_server = *mqtt_addr;
    ssid = *wifissid;
    password = *pass;
    WiFiClient espClient;
    PubSubClient client(espClient);

    client.setServer(mqtt_server, 1883);
}


