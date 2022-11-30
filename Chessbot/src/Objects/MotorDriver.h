/** @file MotorDriver.cpp
 *  @author Dylan Ruiz, created original file
 *  @author Sam Hudson
 *  @author Scott Dunn
 *  @date 2022-Oct-29 Original file by Dylan Ruiz
 */

#ifndef _MOTOR_DRIVER_H_
#define _MOTOR_DRIVER_H_

#include <Arduino.h>
#include "taskshare.h"

class Motor

{

public:
    uint8_t ENABLE_PIN;
    uint8_t STEP_PIN;
    uint8_t DIRECTION_PIN;

    Motor(uint8_t enable_pin, uint8_t step_pin, uint8_t direction_pin);
    Motor(); // Default constructor

    void startMax(int8_t dir, uint16_t steps, Share<bool> &stopFlag);
    void start(float velocity, uint16_t steps, Share<bool> &stopFlag);
};

#endif // _MOTOR_DRIVER_H_
