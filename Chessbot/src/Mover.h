/**
 * @file Mover.h
 * @author Sam Hudson
 * @brief Mover task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#ifndef _MOVER_H
#define _MOVER_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Motor_Driver.h"
#include "APIHandler.h"

class Mover
{
private:
    Motor motor1;
    Motor motor2;
    APIHandler server;
    uint8_t state;
    uint16_t xStep;
    uint16_t yStep;
    bool scanBoard;
    uint8_t xLimPin;
    uint8_t yLimPin;
    uint8_t solenoidPin;
    float omega_Max; // steps per second
    float pitch;     // mm/deg
    float step_size; // deg/step
    float x_coordinate_from;
    float y_coordinate_from;
    float x_coordinate_to;
    float y_coordinate_to;
    float origin_x;
    float origin_y;

public:
    Mover(Motor m1, Motor m2, uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN, APIHandler api); // Constructor
    void run();                                                                          // Method for FSM
    void setState(uint8_t newState);
    void origin();                                    // State 0
    void waiting();                                   // State 1
    void movePiece(float moveFromx, float moveFromy); // State 2
    void grabPiece();                                 // State 3
    void squareOrigin();                              // State 4
    void xGridMove(uint16_t x_to, uint16_t x_from);   // State 5
    void yGridMove(uint16_t y_to, uint16_t y_from);   // State 6
    void gridToCenter();                              // State 7
    void releasePiece();                              // State 8
    void limSwitchRedirection();                      // State 9
    void scan_Board();                                // State 10
};

#endif // _MOVER_H
