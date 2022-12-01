/** @file Motor_Driver.cpp
 *  @author Dylan Ruiz, created original file
 *  @author Sam Hudson
 *  @author Scott Dunn
 *  @date 2022-Oct-29 Original file by Dylan Ruiz
 */

#include <Arduino.h>
#include "Motor_Driver.h"
#include "shares.h"
#include "taskshare.h"

/** This constructor creates an motor object.
 *  @param i2c An I2C object, created as TwoWire(params)
 *  @param address The address to use for the AS5600, default
 */
Motor::Motor(uint8_t enable_pin, uint8_t step_pin, uint8_t direction_pin, uint8_t MS1_pin, uint8_t MS2_pin)
{
    ENABLE_PIN = enable_pin;
    STEP_PIN = step_pin;
    DIRECTION_PIN = direction_pin;
    MS1_PIN = MS1_pin; // manually set to 3.3 on board
    MS2_PIN = MS2_pin; // manually set to gnd on board

    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIRECTION_PIN, OUTPUT);
    pinMode(MS1_PIN, OUTPUT);
    pinMode(MS2_PIN, OUTPUT);
}
/**
 * @brief Default consturctor for a new Motor object (needed for Mover class dependency)
 *
 */
Motor::Motor()
{
    ENABLE_PIN = NULL;
    STEP_PIN = NULL;
    DIRECTION_PIN = NULL;
    MS1_PIN = NULL;
    MS2_PIN = NULL;

    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIRECTION_PIN, OUTPUT);
    pinMode(MS1_PIN, OUTPUT);
    pinMode(MS2_PIN, OUTPUT);
}

/** This method returns the 'raw' angular position measured by the AS5600.
 *  This unscaled and unmodified angle comes out in a 12-bit number.
 *  @return The unscaled position
 */
void Motor::enable(bool motor_enable)
{
    if (motor_enable = true)
    {
        digitalWrite(ENABLE_PIN, LOW);
    }
    else if (motor_enable = false)
    {
        digitalWrite(ENABLE_PIN, HIGH);
    }
}

/** This method returns the scaled and corrected angle measured by the AS5600.
 *  @return Our best estimate of the actual angle, in a 12-bit integer
 */
void Motor::Velocity_MAX(int8_t Dir, uint16_t Steps, Share<bool> &flag)
{
    flag.put(false);

    if (Dir == 1)
    {
        digitalWrite(DIRECTION_PIN, HIGH);
    }
    else if (Dir == -1)
    {
        digitalWrite(DIRECTION_PIN, LOW);
    }
    int i = 0;
    for (i = 0; i < 2 * Steps; i++)
    {

        if (flag.get() == true)
        {
            break;
        }
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        delayMicroseconds(500);
        
    }
    flag.put(true);
}
void Motor::Velocity(float velocity, uint16_t Steps, Share<bool> &flag)
{
    flag.put(false);
    uint32_t delay_time;
    
    if (velocity >= 0)
    {
        digitalWrite(DIRECTION_PIN, HIGH);
    }
    else if (velocity <= 0)
    {
        digitalWrite(DIRECTION_PIN, LOW);
    }

    if (velocity!=0)
    {
        delay_time = 500000 / abs(velocity);
    }
    else 
    {
        delay_time = 1;
    }
    int i = 0;

    for (i = 0; i < 2 * Steps; i++)
    {
        if (flag.get() == true)
        {
            steps_traveled_x.put(i);
            break;
        }
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        delayMicroseconds(delay_time);
    }
    flag.put(true);
}
