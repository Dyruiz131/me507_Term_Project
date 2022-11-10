#include <Arduino.h>
#include "Motor_Driver.h" // Step on rising edge
#include "Mover.h"

#define EN_PIN_1 14   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_1 33 // Step on rising edge
#define DIR_PIN_1 32
#define MS_1_1 34
#define MS_2_1 35

#define EN_PIN_2 13   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_2 25 // Step on rising edge
#define DIR_PIN_2 26
#define MS_1_2 16
#define MS_2_2 17

Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);
Mover mainMover(motor_1, motor_2, 12, 13, 14);
void task_motor1(void *p_params)
{
  while (true)
  {
    motor_1.Velocity_MAX(1, 1000);
    delay(1000);
  }
}

void task_motor2(void *p_params)
{
  while (true)
  {
    motor_2.Velocity(-300, 600);
    delay(1000);
  }
}
void mover_task(void *p_params)
{
  while (true)
  {
    mainMover.run();
    vTaskDelay(100); // Task period
  }
}

void setup()
{
  Serial.begin(115200); // Init used serial port
  while (!Serial)
    ; // Wait for port to be ready

  // Prepare pins
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  // Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
  // Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);

  motor_1.enable(true);
  motor_2.enable(true);
  // motor_1.Velocity_MAX(1,1000);
  // motor_2.Velocity(-300,600);

  xTaskCreate(mover_task, "Mover Task", 4096, NULL, 2, NULL);
  xTaskCreate(task_motor1, "Task Motor1", 2048, NULL, 1, NULL);
  xTaskCreate(task_motor2, "Task Motor2", 2048, NULL, 1, NULL);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // motor_1.Velocity_MAX(1,1000);
  // motor_2.Velocity(-300,600);
  vTaskDelay(60000);
}