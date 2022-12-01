/**
 * @file Mover_Task.h
 * @author Sam Hudson
 * @brief Mover task that manages the chess board movements
 * @version 1.0
 * @date 2022-11-10
 */

#include <Arduino.h>
#include "taskqueue.h"
#include "taskshare.h"
#include "Motor_Driver.h"
#include "Mover.h"
#include "shares.h"
#include "APIHandler.h"
/**
 * @brief Construct a new Mover object
 *
 * @param m1 Motor 1 object
 * @param m2 Motor 2 object
 * @param XLIM_PIN Pin for x limit switch
 * @param YLIM_PIN Pin for y limit switch
 */
#define Solenoid_Pin 11

Mover::Mover(Motor m1, Motor m2, uint8_t XLIM_PIN, uint8_t YLIM_PIN, uint8_t SOLENOID_PIN, APIHandler api)
{
    motor1 = m1;
    motor2 = m2;
    server = api;
    state = 0; // Start state = 0
    uint16_t xStep = 0;
    uint16_t yStep = 0;
    bool scanBoard = true;
    uint8_t xLimPin = XLIM_PIN;
    uint8_t yLimPin = YLIM_PIN;
    uint8_t solenoidPin = SOLENOID_PIN;
    omega_Max = 500; // steps per second
    pitch = 1/9;     // mm/deg
    step_size = .9;  // deg/step
    float origin_x = -5;  // mm to origin of square x-direction
    float origin_y = -5;  // mm to origin of square y-direction
    x_coordinate_from = 0;
    y_coordinate_from = 0;
    x_coordinate_to = 0;
    y_coordinate_to = 0;
}

/**
 * @brief Method called for multitasking. This controls the Mover FSM.
 */
void Mover::run() // Method for FSM
{
    switch (state)
    {
    case 0:
    {
        origin();
        if(scanBoard == true)
        {
            state = 1;              // Wait State
            scanBoard = false;
        }
        else
        {
            state = 9;              // Scan Board State
        }
        break;
    }
    case 1:
    {
        waiting();
        if(Begin_Move.get() == true)
        {
            state = 2;
            Begin_Move.put(false);
        } 
        break;
    }
    case 2:
    {
        x_coordinate_from = Directions_Queue.get();
        y_coordinate_from = Directions_Queue.get();
        x_coordinate_to = Directions_Queue.get();
        y_coordinate_to = Directions_Queue.get();
        movePiece(x_coordinate_from, y_coordinate_from);
        while(true)
        {
            if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Stop_Motor1.put(false);
                Stop_Motor2.put(false);
                state = 3;
                break;
            }
            delay(10);
        } 
        break;
    }
    case 3:
    {
        grabPiece();
        state = 4;
        break;
    }
    case 4:
    {
        squareOrigin();
        while(true)
        {
            if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Stop_Motor1.put(false);
                Stop_Motor2.put(false);
                state = 5;
                break;
            }
            delay(10);
        }
        break;
    }
    case 5:
    {
        xGridMove(x_coordinate_to, x_coordinate_from); 
        while(true)
        {
            if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Stop_Motor1.put(false);
                Stop_Motor2.put(false);
                state = 6;
                break;
            }
            delay(10);
        }
        break;
    }
    case 6:
    {
        yGridMove(y_coordinate_to, y_coordinate_from);
        while(true)
        {
            if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Stop_Motor1.put(false);
                Stop_Motor2.put(false);
                state = 7;
                break;
            }
            delay(10);
        }
        break;
    }
    case 7:
    {
        gridToCenter();
         while(true)
        {
            if(Stop_Motor1.get() == true, Stop_Motor2.get() == true)
            {
                Stop_Motor1.put(false);
                Stop_Motor2.put(false);
                state = 8;
                break;
            }
            delay(10);
        }
        break;
    }
    case 8:
    {
        releasePiece();
        state = 0;
        break;
    }
    case 9:
    {
        scan_Board();
        while(true)
        {
            if(Scan_Board.get() == false)
            {
                break;
            }
            delay(10);

        }
        scanBoard = true;
        state = 0;
        break;
    }
    }
}

void Mover::setState(uint8_t newState)
{
    state = newState;
}

void Mover::origin() // State 0
{
    /* Check x axis */
    while (1)
    {
        if (!digitalRead(xLimPin)) // If x lim switch is hit, stop moving.
        {
            xStep = 0; // Set x position to 0
            break;     // End while loop
        }
        else
        {
            // Move left 1 step
            motor1.Velocity_MAX(-1, 1,Stop_Motor1);
            motor2.Velocity_MAX(-1, 1,Stop_Motor2);
        }
    }
    /* Check y axis */
    while (1)
    {
        if (!digitalRead(yLimPin)) // If y lim switch is hit, stop moving.
        {
            yStep = 0; // Set y position to 0
            break;     // End while loop
        }
        else
        {
            // Move down 1 step
            motor1.Velocity_MAX(1, 1,Stop_Motor1);
            motor2.Velocity_MAX(-1, 1,Stop_Motor2);
        }
    }
}
void Mover::waiting()                             // State 1
{
    
}

void Mover::movePiece(float move_From_x, float move_From_y) // State 2
{
    float Dx = move_From_x; //mm
    float Dy = move_From_y; //mm
    float omega_Lesser = -omega_Max*(2*Dy/Dx - 1)/(2*Dy/Dx + 1);
    uint16_t Num_Step_G = 2*Dy/((1+((Dy - 0.5*Dx)/(Dy + 0.5*Dx)))*step_size*pitch); //Num of steps for greater motor
    uint16_t Num_Step_L = 2*Dy/(step_size*pitch) - Num_Step_G;
    Avel1.put(omega_Lesser);
    Steps1.put(Num_Step_L);
    Motor2_dir.put(1);
    Steps2.put(Num_Step_G);
    Motor1_Start.put(true);
    Motor2Max_Start.put(true);
}
void Mover::grabPiece()     // State 3
{
    digitalWrite(Solenoid_Pin,HIGH);
}                               

void Mover::squareOrigin()                        // State 4
{
    float omega_Lesser = omega_Max*(2*origin_y/origin_x - 1)/(2*origin_y/origin_x + 1);
    uint16_t Num_Step_G = 2*origin_y/((1+((origin_y - 0.5*origin_x)/(origin_y + 0.5*origin_x)))*step_size*pitch); //Num of steps for greater motor
    uint16_t Num_Step_L = 2*origin_y/(step_size*pitch) - Num_Step_G;
    Avel1.put(omega_Lesser);
    Steps1.put(Num_Step_L);
    Motor2_dir.put(-1);
    Steps2.put(Num_Step_G);
    Motor1_Start.put(true);
    Motor2Max_Start.put(true);
}

void Mover::xGridMove(uint16_t x_to, uint16_t x_from)   // State 5
{
    int16_t x_move = x_to - x_from; // mm
    uint16_t Num_Step = x_move/(pitch*step_size);
    int8_t direction = 0;
    if(x_move < 0)
    {
        direction = -1;
    }
    else
    {
        direction = 1;
    }
    Steps1.put(Num_Step);
    Steps2.put(Num_Step);
    Motor1_dir.put(direction);
    Motor2_dir.put(direction);
    Motor1Max_Start.put(true);
    Motor2Max_Start.put(true);
}

void Mover::yGridMove(uint16_t y_to, uint16_t y_from)  // State 6
{
    int16_t y_move = y_to - y_from; // mm
    uint16_t Num_Step = y_move/(pitch*step_size);
    int8_t direction_1 = 0;
    int8_t direction_2 = 0;
    if(y_move < 0)
    {
        direction_1 = 1;
        direction_2 = -1;
    }
    else
    {
        direction_1 = -1;
        direction_2 = 1;
    }
    Steps1.put(Num_Step);
    Steps2.put(Num_Step);
    Motor1_dir.put(direction_1);
    Motor2_dir.put(direction_2);
    Motor1Max_Start.put(true);
    Motor2Max_Start.put(true);
}
void Mover::gridToCenter()                             // State 7
    {
        float omega_Lesser = -omega_Max*(2*origin_y/origin_x - 1)/(2*origin_y/origin_x + 1);
        uint16_t Num_Step_G = 2*origin_y/((1+((origin_y - 0.5*origin_x)/(origin_y + 0.5*origin_x)))*step_size*pitch); //Num of steps for greater motor
        uint16_t Num_Step_L = 2*origin_y/(step_size*pitch) - Num_Step_G;
        Avel1.put(omega_Lesser);
        Steps1.put(Num_Step_L);
        Motor2_dir.put(1);
        Steps2.put(Num_Step_G);
        Motor1_Start.put(true);
        Motor2Max_Start.put(true);  
    }
void Mover::releasePiece()                              // State 8
    {
        digitalWrite(Solenoid_Pin,LOW);
    }

void Mover::scan_Board()                                 // State 9
    {
        Scan_Board.put(true);
    }

