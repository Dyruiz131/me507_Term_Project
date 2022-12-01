/**
 * @file Task_ScanBoard.cpp
 * @author Dylan Ruiz
 * @brief ScanBoard task that scans the pieces of the board
 * @version 1.0
 * @date 2022-16-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Task_ScanBoard.h"
#include "shares.h"


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

#define Light_Pin 10
Task_ScanBoard::Task_ScanBoard()
{ 
 num_steps = 1000; // Steps to make 1 cross of board
 Frequency = 100; // Frequency of light detection (hz) 
 steps_first = 50; // Steps to for first row 
 steps_rows = 100; // Steps Between Rows
 length_array_light = 200;
 length_array_piece = 64;
}

/**
 * @brief Method called for multitasking. This controls the Mover FSM.
 */
void Task_ScanBoard::run() // Method for FSM
{
   

    switch (state)
    {
    case 0:
    {
        Waiting();
        if(Scan_Board.get() == true)
        {
            state = 1;                
        }
        break;
    }
    case 1:
    {
        Scan();
        Scan_Board.put(false); 
        state = 0;
        break;
    }
  
    }
}

void Task_ScanBoard::Waiting()  //Waiting
{
    
} 
void Task_ScanBoard::Scan() // State 2 
{
    int direction = -1;

    Steps1.put(steps_first);
    Steps2.put(steps_first);
    Motor1_dir.put(1);
    Motor2_dir.put(-1);
    Motor1Max_Start.put(true);
    Motor2Max_Start.put(true);
    for(int loop = 0; loop<8; loop++)
    {
        while (true)
        {
            if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Steps1.put(num_steps);
                Steps2.put(num_steps);
                Motor1_dir.put(direction);
                Motor2_dir.put(direction); 
                Motor1Max_Start.put(true);
                Motor2Max_Start.put(true); 
                break;
            }
        }

        for(count=0; count<length_array_light; count++)
        {
            array_light[count] = digitalRead(Light_Pin);
            vTaskDelay(1000/Frequency);

        }
        for(count = 0; count<8; count++)
        {
            int threshold = 0;

            for(count2=0; count<length_array_light/8; count++);
            {
                int index = count*length_array_light/8+count2;
                if(array_light[index] == 1)
                {
                    threshold += 1;
                } 
            }
            if(threshold > 4)
            {
                array_piece[loop*8+count] = true;
            } 
            else
            {
                array_piece[loop*8+count] = false;
            }
        }
        while(true)
        {
            if (loop == 7)
            {
                break;
            }
            else if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Steps1.put(steps_rows);
                Steps2.put(steps_rows);
                Motor1_dir.put(1);
                Motor2_dir.put(-1);
                Motor1Max_Start.put(true);
                Motor2Max_Start.put(true);
                break;
            }
            
        }
        direction = direction * -1;
    }

}
                     