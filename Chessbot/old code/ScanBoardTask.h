/**
 * @file ScanBoardTask.h
 * @author Dylan Ruiz
 * @brief ScanBoard task that scans the pieces of the board
 * @version 1.0
 * @date 2022-11-26
 */

#ifndef _SCAN_BOARD_TASK_H
#define _SCAN_BOARD_TASK_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "objects/MotorDriver.h"

class ScanBoardTask
{
private:
    Motor motor;
    uint8_t state;
    uint16_t num_steps;   // Steps to make 1 cross of board
    uint16_t Frequency;   // Frequency of light detection (hz)
    uint16_t steps_first; // Steps to for first row
    uint16_t steps_rows;  // Steps Between Rows
    int count;
    int array_light[200];
    bool array_piece[64];
    int count2;
    int length_array_light;
    int length_array_piece;

public:
    ScanBoardTask(); // Constructor
    void run();          // Method for FSM
    void waiting();
    void Scan();
};

#endif // _SCAN_BOARD_TASK_H