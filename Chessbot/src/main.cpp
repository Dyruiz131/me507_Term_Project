#include <Arduino.h>
#include "Motor_Driver.h" 								// Step on rising edge
#include "taskshare.h"                    // Header for inter-task shared data
#include "taskqueue.h"                    // Header for inter-task data queues
#include "shares.h"                       // Header for shares used in this project

#define EN_PIN_1    14 								// LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_1  33 								// Step on rising edge
#define DIR_PIN_1   32    
#define MS_1_1      23
#define MS_2_1      22 

#define EN_PIN_2    13 								// LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_2  25 								// Step on rising edge
#define DIR_PIN_2   26          
#define MS_1_2      16
#define MS_2_2      17

#define BUTTON_PIN  15

Share<bool> Motor1_Done ("Motor1");
Share<bool> Motor2_Done ("Motor2");

Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);
void task_motor1 (void* p_params)
{
  while (true)
  {
    motor_1.Velocity_MAX(1,1000, Motor1_Done);
    delay(1000);
  }
}

void task_motor2 (void* p_params)
{
  while (true)
  {
    motor_2.Velocity(-300,600, Motor2_Done);
    delay(1000);
  }
}

void task_kill (void* p_params)
{
  while (true)
  {
    if (digitalRead(BUTTON_PIN) == 0)
    {
      Serial << "button was pressed, state low: 0" ;
      Motor1_Done.put(true);
      Motor2_Done.put(true);
      delay(100); 
    }
    else
    {
      Motor1_Done.put(false);
      Motor2_Done.put(false);
      delay(100);
    }
    
  }
}
void setup() {
  	Serial.begin(115200);							// Init used serial port
	  while(!Serial);									// Wait for port to be ready

	  // Prepare pins
	  pinMode(EN_PIN_1, OUTPUT);
	  pinMode(STEP_PIN_1, OUTPUT);
    pinMode(MS_1_1, OUTPUT);
    pinMode(MS_2_1, OUTPUT);
    pinMode(MS_1_2, OUTPUT);
    pinMode(MS_2_2, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    digitalWrite(MS_1_1, HIGH);
    digitalWrite(MS_2_1, LOW);
    digitalWrite(MS_1_2, HIGH);
    digitalWrite(MS_2_2, LOW);
    //Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
    //Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);
    
    motor_1.enable(true);
    motor_2.enable(true);
    // motor_1.Velocity_MAX(1,1000);
    // motor_2.Velocity(-300,600);

    
    xTaskCreate (task_motor1, "Task Motor1", 2048, NULL, 1, NULL);
    xTaskCreate (task_motor2, "Task Motor2", 2048, NULL, 1, NULL);
    xTaskCreate (task_kill, "Task Kill", 2048, NULL, 1, NULL);
}


void loop() 
{
  // put your main code here, to run repeatedly:
  // motor_1.Velocity_MAX(1,1000);
  // motor_2.Velocity(-300,600);
  vTaskDelay (60000);
}