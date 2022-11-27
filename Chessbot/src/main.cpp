#include <Arduino.h>
#include "Mover.h"
#include "Motor_Driver.h" // Step on rising edge
#include "taskshare.h"    // Header for inter-task shared data
#include "taskqueue.h"    // Header for inter-task data queues
#include "shares.h"       // Header for shares used in this project
#include "Task_Motor.h"
#include "Task_ScanBoard.h"

#define EN_PIN_1 14   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_1 33 // Step on rising edge
#define DIR_PIN_1 32
#define MS_1_1 23
#define MS_2_1 22

#define EN_PIN_2 13   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_2 25 // Step on rising edge
#define DIR_PIN_2 26
#define MS_1_2 16
#define MS_2_2 17

#define BUTTON_PIN 15

Share<bool> Stop_Motor1("Stop_Motor1");
Share<bool> Stop_Motor2("Stop_Motor2");
Share<bool> Begin_Move("Begin_Move");
Queue<float> Directions_Queue(4, "Directions");
Share<uint16_t> Steps1("Num_of_Step_1");
Share<uint16_t> Steps2("Num_of_Step_2");
Share<float> Avel1("Steps/sec1");
Share<float> Avel2("Steps/sec2");
Share<int8_t> Motor1_dir("Direction_1");
Share<int8_t> Motor2_dir("Direction_2");
Share<bool> Motor1_Start("Motor1_Start");
Share<bool> Motor2_Start("Motor2_Start");
Share<bool> Motor1Max_Start("Motor1Max_Start");
Share<bool> Motor2Max_Start("Motor2Max_Start");
Share<bool> Scan_Board("Scan_Board");

Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);
Mover mainMover(motor_1, motor_2, 12, 13, 14);
Task_Motor task_motor1(motor_1, Stop_Motor1, Motor1_dir, Avel1, Steps1, Motor1_Start, Motor1Max_Start);
Task_Motor task_motor2(motor_2, Stop_Motor2, Motor2_dir, Avel2, Steps2, Motor2_Start, Motor2Max_Start);
Task_ScanBoard scanboard;

// void task_motor1(void *p_params)
// {
//   while (true)
//   {
//     motor_1.Velocity_MAX(1, 1000, Stop_Motor1);
//     delay(1000);
//   }
// }

// void task_motor2(void *p_params)
// {
//   while (true)
//   {
//     motor_2.Velocity(-300, 600, Stop_Motor2);
//     delay(1000);
//   }
// }

void motor_task1(void *p_params)
{
  while (true)
  {
    task_motor1.run();
    vTaskDelay(100); // Task period
  }
}

void motor_task2(void *p_params)
{
  while (true)
  {
    task_motor2.run();
    vTaskDelay(100); // Task period
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

void task_scanboard(void *p_params)
{
  while (true)
  {
    scanboard.run();
    vTaskDelay(100); // Task period
  }
}
void task_kill(void *p_params)
{
  while (true)
  {
    if (digitalRead(BUTTON_PIN) == 0)
    {
      Serial << "button was pressed, state low: 0";
      Stop_Motor1.put(true);
      Stop_Motor2.put(true);
      delay(100);
    }
    else
    {
      Stop_Motor1.put(false);
      Stop_Motor2.put(false);
      delay(100);
    }
  }
}
void setup()
{
  Serial.begin(115200); // Init used serial port
  while (!Serial)
  {
  } // Wait for port to be ready

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

  // Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
  // Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);

  // motor_1.enable(true);
  // motor_2.enable(true);
  // motor_1.Velocity_MAX(1,1000);
  // motor_2.Velocity(-300,600);

  xTaskCreate(motor_task1, "Task Motor1", 2048, NULL, 1, NULL);
  xTaskCreate(motor_task2, "Task Motor2", 2048, NULL, 1, NULL);
  xTaskCreate(task_kill, "Task Kill", 2048, NULL, 1, NULL);
  xTaskCreate(mover_task, "Mover Task", 4096, NULL, 2, NULL);
  xTaskCreate(task_scanboard, "Scan Task", 4096, NULL, 2, NULL);
  // xTaskCreate(task_motor1, "Task Motor1", 2048, NULL, 1, NULL);
  // xTaskCreate(task_motor2, "Task Motor2", 2048, NULL, 1, NULL);
}

void loop()
{
  vTaskDelay(60000);
}