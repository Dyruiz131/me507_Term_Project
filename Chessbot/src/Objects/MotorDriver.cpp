/** @file Motor_Driver.cpp
 *  @author Dylan Ruiz, created original file
 *  @author Sam Hudson
 *  @author Scott Dunn
 *  @date 2022-Oct-29 Original file by Dylan Ruiz
 */

#include <Arduino.h>
#include "objects/MotorDriver.h"
#include "shares.h"
#include "taskshare.h"

/** This constructor creates an motor object.
 *  @param i2c An I2C object, created as TwoWire(params)
 *  @param address The address to use for the AS5600, default
 */
Motor::Motor(uint8_t en, uint8_t step, uint8_t dir)
{
    ENABLE_PIN = en;
    STEP_PIN = step;
    DIRECTION_PIN = dir;

    // Setup pins
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIRECTION_PIN, OUTPUT);
}
/**
 * @brief Default consturctor for a new Motor object (needed for motor task class dependency)
 *
 */
Motor::Motor()
{
    ENABLE_PIN = 0;
    STEP_PIN = 0;
    DIRECTION_PIN = 0;

    // Setup pins
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIRECTION_PIN, OUTPUT);
}

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

void Motor::maxVelocity(int8_t Dir, uint16_t Steps, Share<bool> &flag)
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
        delay(1);
    }
    flag.put(true);
}
void Motor::velocity(float velocity, uint16_t Steps, Share<bool> &flag)
{
    flag.put(false);

    if (velocity >= 0)
    {
        digitalWrite(DIRECTION_PIN, HIGH);
    }
    else if (velocity <= 0)
    {
        digitalWrite(DIRECTION_PIN, LOW);
    }

    float delay_time = 1000000 / abs(velocity);
    int i = 0;

    for (i = 0; i < 2 * Steps; i++)
    {
        if (flag.get() == true)
        {
            break;
        }
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        delayMicroseconds(delay_time);
    }
    flag.put(true);
}
