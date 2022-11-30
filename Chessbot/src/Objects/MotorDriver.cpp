/** @file MotorDriver.cpp
 *  @author Dylan Ruiz, created original file
 *  @author Sam Hudson
 *  @author Scott Dunn
 *  @date 2022-Oct-29 Original file by Dylan Ruiz
 */

#include <Arduino.h>
#include "objects/MotorDriver.h"
#include "shares.h"


Motor::Motor(uint8_t en, uint8_t step, uint8_t dir)
{
    ENABLE_PIN = en;
    STEP_PIN = step;
    DIRECTION_PIN = dir;

    // Setup pins
    pinMode(ENABLE_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIRECTION_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW); // Enable motor
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
    digitalWrite(ENABLE_PIN, LOW); // Enable motor
}

void Motor::startMax(int8_t Dir, uint16_t Steps, Share<bool> &stopFlag)
{
    stopFlag.put(false); // Start motor

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

        if (stopFlag.get() == true)
        {
            break;
        }
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        delay(1);
    }
    stopFlag.put(true); // Stop motor
}
void Motor::start(float velocity, uint16_t Steps, Share<bool> &stopFlag)
{
    stopFlag.put(false); //Start motor

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
        if (stopFlag.get() == true)
        {
            break;
        }
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        delayMicroseconds(delay_time);
    }
    stopFlag.put(true); // Stop motor
}
