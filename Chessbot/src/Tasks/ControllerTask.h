/**
 * @file Mover.h
 * @author Sam Hudson
 * @brief Controller task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#ifndef _CONTROLLER_TASK_H_
#define _CONTROLLER_TASK_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "objects/MotorDriver.h"
#include "objects/APIHandler.h"
#include "kinematics.h"

class Controller
{
private:
    APIHandler server;
    uint8_t state;
    uint16_t xStep;
    uint16_t yStep;
    uint8_t xLimPin;
    uint8_t yLimPin;
    uint8_t solenoidPin;
    uint8_t sensorPin;
    float omegaMax; // steps per second    
    float stepLength; // deg/step
    float xCoordinateFrom;
    float yCoordinateFrom;
    float xCoordinateTo;
    float yCoordinateTo;
    float xOrigin;
    float yOrigin;
    float takePiece;
    float xPieceGraveyard;
    float yPieceGraveyard;
    float sensorOffset;
    uint8_t count = 0;
    Kinematics kinematics;
    float moveTake;

public:
    Controller(uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN, uint8_t SENSOR_PIN); // Constructor
    void run();                                                                                               // Method for FSM
    void setState(uint8_t newState);
    void origin_x();                                    // State 0
    void origin_y();
    void movePiecex(float moveFromx, float moveFromy); // State 2
    void movePiecey(float moveFromx, float moveFromy); // State 2
    void grabPiece();                                 // State 3
    void centerToGridx();                              // State 4
    void centerToGridy();
    void gridToGraveyardx();
    void gridToGraveyardy();
    void xGridMove(uint16_t x_to, uint16_t x_from); // State 5
    void yGridMove(uint16_t y_to, uint16_t y_from); // State 6
    void gridToCenterx();                            // State 7
    void gridToCentery();
    void releasePiece();                            // State 8
    void limSwitchRedirection();                    // State 9
    void runScan();                                 // State 10
    bool detectPiece();
};
#endif // _CONTROLLER_TASK_H_
