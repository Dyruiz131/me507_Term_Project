/**
 * @file ScanBoardTask.cpp
 * @author Dylan Ruiz
 * @brief ScanBoard task that scans the pieces of the board
 * @version 1.0
 * @date 2022-16-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "tasks/ScanBoardTask.h"
#include "shares.h"

/**
 *
 * @brief Construct a new Controller object
 *
 * @param motor1 Motor 1 object
 * @param Stop_Motor1 Share object used as a flag for stopping the motor
 * @param direction1 Share object used to indicate direction of the motor
 * @param velocity1 Share object used to input velocity for motor function
 * @param steps1 Share object used to input number of steps for motor function
 * @param Motor_start1 Share object used as a flag to progress FSM to state 1
 * @param MotorMax_Start1 Share object used as a flag to progress FSM to state 2
 */

#define Light_Pin 10
ScanBoardTask::ScanBoardTask()
{
    num_steps = 1000; // Steps to make 1 cross of board
    Frequency = 100;  // Frequency of light detection (hz)
    steps_first = 50; // Steps to for first row
    steps_rows = 100; // Steps Between Rows
    length_array_light = 200;
    length_array_piece = 64;
}

/**
 * @brief Method called for multitasking. This controls the Controller FSM.
 */
void ScanBoardTask::run() // Method for FSM
{

    switch (state)
    {
    case 0:
    {
        waiting();
        if (scanBoard.get() == true)
        {
            state = 1;
        }
        break;
    }
    case 1:
    {
        Scan();
        scanBoard.put(false);
        state = 0;
        break;
    }
    }
}

void ScanBoardTask::waiting() // waiting
{
}
void ScanBoardTask::Scan() // State 2
{
    int direction = -1;

    steps1.put(steps_first);
    steps2.put(steps_first);
    dirMotor1.put(1);
    dirMotor2.put(-1);
    startMaxMotor1.put(true);
    startMaxMotor2.put(true);
    for (int loop = 0; loop < 8; loop++)
    {
        while (true)
        {
            if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                steps1.put(num_steps);
                steps2.put(num_steps);
                dirMotor1.put(direction);
                dirMotor2.put(direction);
                startMaxMotor1.put(true);
                startMaxMotor2.put(true);
                break;
            }
        }

        for (count = 0; count < length_array_light; count++)
        {
            array_light[count] = digitalRead(Light_Pin);
            vTaskDelay(1000 / Frequency);
        }
        for (count = 0; count < 8; count++)
        {
            int threshold = 0;

            for (count2 = 0; count < length_array_light / 8; count++)
                ;
            {
                int index = count * length_array_light / 8 + count2;
                if (array_light[index] == 1)
                {
                    threshold += 1;
                }
            }
            if (threshold > 4)
            {
                array_piece[loop * 8 + count] = true;
            }
            else
            {
                array_piece[loop * 8 + count] = false;
            }
        }
        while (true)
        {
            if (loop == 7)
            {
                break;
            }
            else if (stopMotor1.get() == true, stopMotor2.get() == true)
            {
                steps1.put(steps_rows);
                steps2.put(steps_rows);
                dirMotor1.put(1);
                dirMotor2.put(-1);
                startMaxMotor1.put(true);
                startMaxMotor2.put(true);
                break;
            }
        }
        direction = direction * -1;
    }
}
