/**
 * @file Task_Motor.h
 * @author Dylan Ruiz
 * @brief Motor task that manages actuation for the Motors.
 * @version 1.0
 * @date 2022-11-23
 */

#ifndef _TASK_MOTOR_H
#define _TASK_MOTOR_H

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Motor_Driver.h"

class Task_Motor
{
private:
    Motor motor;
    uint8_t state;
    Share<bool> Stop_Motor;
    Share<int8_t> direction;
    Share<float> velocity;
    Share<uint16_t> steps;
    Share<bool>  Motor_start;
    Share<bool>  MotorMax_Start;
    
    
public:
    Task_Motor(Motor motor1, Share<bool>& Stop_Motor, Share<int8_t>& direction, Share<float>& velocity, Share<uint16_t>& steps, Share<bool>& Motor_start, Share<bool>& MotorMax_Start);   // Constructor
    void run();             // Method for FSM
    void Waiting();
    void Motor_run(); // Method for velocity specific motor actuation
    void Motor_Max_run(); // Method for Max velocity motor actuation
  
};

#endif // _TASK_MOTOR_H