/**
 * @file APIHandler.h
 * @author Sam Hudson
 * @brief Handles the API communication between the server and the chess board
 * @version 1.0
 * @date 2022-11-27
 */
#include "APIHandler.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// SSL Certificate (API uses HTTPS)
const char *SSLCertificate =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ \n"
    "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD \n"
    "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX \n"
    "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y \n"
    "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy \n"
    "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr \n"
    "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr \n"
    "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK \n"
    "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu \n"
    "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy \n"
    "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye \n"
    "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1 \n"
    "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3 \n"
    "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92 \n"
    "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx \n"
    "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0 \n"
    "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz \n"
    "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS \n"
    "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
    "-----END CERTIFICATE-----\n";

/**
 * @brief Constructs a new APIHandler::APIHandler object
 * and initialises the WiFi connection.
 * @param wifiSSID The SSID for the WiFi network
 * @param pass The password for the WiFi network
 */
APIHandler::APIHandler(const char *wifiSSID, const char *pass)
{
    Serial.begin(115200);
    ssid = wifiSSID;
    password = pass;
    certificate = SSLCertificate;

    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

APIHandler::APIHandler()
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
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
    sendPOST("https://chessbotapi.onrender.com/fen", certificate, req);
}

/**
 * @brief Gets the current FEN string from the server
 *
 * @return String of the current FEN string
 */
String APIHandler::getFen()
{
    String res = sendGET("https://chessbotapi.onrender.com/fen", certificate);
    JSONVar jsonRes = JSON.parse(res);
    String fen = jsonRes["fen"];
    return fen;
}

/**
 * @brief Gets the current turn from the server
 *
 * @return char of the current turn colour
 */
char APIHandler::getTurn()
{
    String res = sendGET("https://chessbotapi.onrender.com/fen", certificate);
    JSONVar jsonRes = JSON.parse(res);
    String turn = jsonRes["turn"];
    return turn[0];
}

/**
 * @brief Gets the most recent move from the server
 *
 * @return String of the most recent move in the format "e2,e4"
 */
String APIHandler::getLastMove()
{
    String res = sendGET("https://chessbotapi.onrender.com/lastMove", certificate);
    JSONVar jsonRes = JSON.parse(res);
    String lastMove = jsonRes["lastMove.from"] + "," + jsonRes["lastMove.to"];
    return lastMove;
}
/**
 * @brief Gets the current move status from the server
 *
 * @return boolean of whether the bot has completed its move
 */
boolean APIHandler::getMoveStatus()
{
    String res = sendGET("https://chessbotapi.onrender.com/moveComplete", certificate);
    JSONVar jsonRes = JSON.parse(res);
    boolean status = jsonRes["moveComplete"];
    return status;
}

/**
 * @brief Sets the current move stat from the server
 *
 */
void APIHandler::sendMoveStatus(boolean status)
{
    String req = "{\"moveComplete\":" + String(status) + "}";
    sendPOST("https://chessbotapi.onrender.com/moveComplete", certificate, req);
}
