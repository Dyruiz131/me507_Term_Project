#include <Arduino.h>
#include "Mover.h"
#include "Motor_Driver.h" // Step on rising edge
#include "taskshare.h"    // Header for inter-task shared data
#include "taskqueue.h"    // Header for inter-task data queues
#include "shares.h"       // Header for shares used in this project
#include "Task_Motor.h"
#include "Task_ScanBoard.h"
//#include "APIHandler.h"

// motor1
#define EN_PIN_1 14   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_1 33 // Step on rising edge
#define DIR_PIN_1 32
#define MS_1_1 23
#define MS_2_1 22

// motor2
#define EN_PIN_2 13   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_2 26 // Step on rising edge
#define DIR_PIN_2 25
#define MS_1_2 16
#define MS_2_2 17

//#define BUTTON_PIN 15
#define XLIM_PIN 2
#define YLIM_PIN 15
#define SOLENOID_PIN 4


Share<bool> Stop_Motor1("Stop_Motor1");
Share<bool> Stop_Motor2("Stop_Motor2");
Share<uint32_t> steps_traveled_x("steps_x");
// Share<bool> Begin_Move("Begin_Move");
// Queue<float> Directions_Queue(4, "Directions");
// Share<uint16_t> Steps1("Num_of_Step_1");
// Share<uint16_t> Steps2("Num_of_Step_2");
// Share<float> Avel1("Steps/sec1");
// Share<float> Avel2("Steps/sec2");
// Share<int8_t> Motor1_dir("Direction_1");
// Share<int8_t> Motor2_dir("Direction_2");
// Share<bool> Motor1_Start("Motor1_Start");
// Share<bool> Motor2_Start("Motor2_Start");
// Share<bool> Motor1Max_Start("Motor1Max_Start");
// Share<bool> Motor2Max_Start("Motor2Max_Start");
// Share<bool> Scan_Board("Scan_Board");

// API setup for dependency injection into Mover object
// const char *ssid = "SSID";
// const char *password = "PASSWORD";
// APIHandler apiHandler(ssid, password);

// Motor setup
Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);

// Dependency injection for board
//Mover mainMover(motor_1, motor_2, XLIM_PIN, YLIM_PIN, SOLENOID_PIN, apiHandler);

// Create motor task objects
//Task_Motor task_motor1(motor_1, Stop_Motor1, Motor1_dir, Avel1, Steps1, Motor1_Start, Motor1Max_Start);
//Task_Motor task_motor2(motor_2, Stop_Motor2, Motor2_dir, Avel2, Steps2, Motor2_Start, Motor2Max_Start);

// Create scan board task object
//Task_ScanBoard scanboard;

void task_motor1(void *p_params)
{
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 8000;
  //vTaskDelay(5000);
  bool solenoidDown = true;
  while (true)
  {
    xLastWakeTime = xTaskGetTickCount();
    motor_1.Velocity(-364, 364*2, Stop_Motor1); // when positive direction is CW
    if(solenoidDown)
    {
      // uint32_t the_steps = steps.get();
      digitalWrite(SOLENOID_PIN, HIGH);
    }
    else
    {
      digitalWrite(SOLENOID_PIN, LOW);
    }
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    //Serial.println(steps_traveled_x.get());
    xLastWakeTime = xTaskGetTickCount();
    solenoidDown = !solenoidDown;
    motor_1.Velocity(364, 364*2, Stop_Motor1);
    //Serial.println("done2");
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}

void task_motor2(void *p_params)
{
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = 8000;
  //vTaskDelay(5000);
  while (true)
  {
    xLastWakeTime = xTaskGetTickCount();
    motor_2.Velocity(1000, 2000, Stop_Motor2);
    //Serial << steps.get() << endl;
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    xLastWakeTime = xTaskGetTickCount();
    motor_2.Velocity(-1000, 2000, Stop_Motor2);
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
  }
}


void task_kill(void *p_params)
{
  while (true)
  {
    if (digitalRead(YLIM_PIN) == 0)
    {
      //Serial << "Y button was pressed, state low: 0";
      Stop_Motor1.put(true);
      Stop_Motor2.put(true);
      delay(10);
      Stop_Motor1.put(false);
      Stop_Motor2.put(false);
      delay(3000);
    }
    else
    {
      Stop_Motor1.put(false);
      Stop_Motor2.put(false);
      delay(10);
    }
    if (digitalRead(XLIM_PIN) == 0)
    {
      //Serial << "X button was pressed, state low: 0";
      Stop_Motor1.put(true);
      Stop_Motor2.put(true);
      delay(10);
      Stop_Motor1.put(false);
      Stop_Motor2.put(false);
      delay(3000);
    }
    else
    {
      Stop_Motor1.put(false);
      Stop_Motor2.put(false);
      delay(10);
    }
    delay(10);
  }
}

/* End of task definitions for FreeRTOS*/

/**
 * @brief Main program that sets up FreeRTOS tasks and starts the scheduler
 *
 */
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
  pinMode(YLIM_PIN, INPUT_PULLUP);
  pinMode(XLIM_PIN, INPUT_PULLUP);
  pinMode(SOLENOID_PIN, OUTPUT);
  digitalWrite(MS_1_1, HIGH);
  digitalWrite(MS_2_1, LOW);
  digitalWrite(MS_1_2, HIGH);
  digitalWrite(MS_2_2, LOW);
  digitalWrite(SOLENOID_PIN, LOW);

  //xTaskCreate(motor_task1, "Task Motor1", 2048, NULL, 1, NULL);
  //xTaskCreate(motor_task2, "Task Motor2", 2048, NULL, 1, NULL);
  xTaskCreate(task_kill, "Task Kill", 4096, NULL, 1, NULL);
  //xTaskCreate(mover_task, "Mover Task", 4096, NULL, 2, NULL);
  //xTaskCreate(task_scanboard, "Scan Task", 4096, NULL, 2, NULL);
  xTaskCreate(task_motor1, "Task Motor1", 4096, NULL, 1, NULL);
  xTaskCreate(task_motor2, "Task Motor2", 4096, NULL, 1, NULL);

  //  
}

/**
 * @brief Loop that runs every 60000 ms to keep FreeRTOS from crashing
 *
 */
void loop()
{
  vTaskDelay(60000);
}