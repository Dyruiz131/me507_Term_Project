/**
 * @file Task_Motor.cpp
 * @author Dylan Ruiz
 * @brief Motor task that manages the motor movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Motor_Driver.h"
#include "Task_Motor.h"

#define Solenoid_Pin 11

/**
 * 
 * @brief Construct a new Mover object
 *
 * @param motor1 Motor 1 object
 * @param Stop_Motor1 Share object used as a flag for stopping the motor
 * @param direction1 Share object used to indicate direction of the motor
 * @param velocity1 Share object used to input velocity for motor function
 * @param steps1 Share object used to input number of steps for motor function
 * @param Motor_start1 Share object used as a flag to progress FSM to state 1
 * @param MotorMax_Start1 Share object used as a flag to progress FSM to state 2
 */

Task_Motor::Task_Motor(Motor motor1, Share<bool>& Stop_Motor1, Share<int8_t>& direction1, Share<float>& velocity1, Share<uint16_t>& steps1, Share<bool>& Motor_start1, Share<bool>& MotorMax_Start1 )
{ 
    motor = motor1;
    Stop_Motor = Stop_Motor1;
    direction = direction1;
    velocity = velocity1;
    steps = steps1;
    Motor_start = Motor_start1;
    MotorMax_Start = MotorMax_Start1;
    state = 0; // Start state = 0
    motor.enable(true);
}

/**
 * @brief Method called for multitasking. This controls the Mover FSM.
 */
void Task_Motor::run() // Method for FSM
{
   

    switch (state)
    {
    case 0:
    {
        Waiting();
        if(Motor_start.get() == true)
        {
            state = 1;                  // 
            Motor_start.put(false); 
        }
        else if(MotorMax_Start.get() == true)
        {
            state = 2;              // Scan Board State
            MotorMax_Start.put(false);
        }
        break;
    }
    case 1:
    {
        Motor_run();
        state = 0;
        break;
    }
    case 2:
    {
        Motor_Max_run();
        state = 0;
        break;
    }
 
    }
}

void Task_Motor::Waiting()  //Waiting
{
    
} 
void Task_Motor::Motor_run() // State 2 
{
    motor.Velocity(velocity.get(),steps.get(), Stop_Motor);
}
void Task_Motor::Motor_Max_run()     // State 3
{
    motor.Velocity_MAX(direction.get(), steps.get(), Stop_Motor);
}                               
