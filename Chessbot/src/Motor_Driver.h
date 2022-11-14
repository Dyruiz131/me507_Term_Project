/** @file Motor_Driver.h
 *  This file contains a method which prints an array.
 * 
 *  @author Dylan Ruiz
 *  @date   2022-Oct-26 Original file
 
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
uint8_t MS1_PIN;
uint8_t MS2_PIN;

    Motor(uint8_t enable_pin, uint8_t step_pin, uint8_t direction_pin, uint8_t MS1_pin, uint8_t MS2_pin);
    Motor(); //Default constructor

    void enable (bool enable_motor);

    void Velocity_MAX (int8_t Dir, uint16_t Steps, Share<bool>& flag);

    void Velocity (float velocity, uint16_t Steps, Share<bool>& flag);
};

#endif // _MOTOR_DRIVER_H_
