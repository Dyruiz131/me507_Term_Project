#include <Arduino.h>
#include "Tasks/ControllerTask.h"
#include "Objects/MotorDriver.h"
#include "taskshare.h" // Header for inter-task shared data
#include "taskqueue.h" // Header for inter-task data queues
#include "shares.h"    // Header for shares used in this project
#include "Tasks/MotorTask.h"
#include "Tasks/ScanBoardTask.h"
#include "Objects/APIHandler.h"
#include "wifiPass.h"

// Motor 1 pins
#define EN_PIN_1 14   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_1 33 // Step on rising edge
#define DIR_PIN_1 32

// Motor 2 pins
#define EN_PIN_2 13   // LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_2 25 // Step on rising edge
#define DIR_PIN_2 26

// Limit switch pins
#define XLIM_PIN 12
#define YLIM_PIN 13

// Solenoid pin
#define SOLENOID_PIN 14

/* Define Shares*/
Share<bool> stopMotor1("stopMotor1");
Share<bool> stopMotor2("stopMotor2");
Share<bool> beginMove("beginMove");
Queue<float> directionsQueue(8, "directionsQueue");
Share<uint16_t> steps1("No. of steps1");
Share<uint16_t> steps2("No. of steps2");
Share<float> aVel1("Steps/sec1");
Share<float> aVel2("Steps/sec2");
Share<int8_t> dirMotor1("Motor1 Direction");
Share<int8_t> dirMotor2("Motor2 Direction");
Share<bool> startMotor1("Start Motor1");
Share<bool> startMotor2("Start Motor2");
Share<bool> startMaxMotor1("Start Max Motor1");
Share<bool> startMaxMotor2("Start Max Motor2");
Share<bool> scanBoard("Scan Board");

// WiFi credentials (ssid and password is ignored by git for security)
const char *ssid = WIFI_SSID;                 // Import SSID from wifi.h
const char *password = WIFI_PASSWORD;         // Import password from wifi.h
const char *SSLCertificate = SSL_CERTIFICATE; // Import SSL certificate from wifi.h

// API setup for dependency injection into Controller object
APIHandler apiHandler(ssid, password, SSLCertificate);

// Motor setup
Motor motor1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1);
Motor motor2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2);

// Dependency injection for board
Controller mainController(XLIM_PIN, YLIM_PIN, SOLENOID_PIN, apiHandler);

// Create motor task objects
MotorTask motorTask1(motor1, stopMotor1, dirMotor1, aVel1, steps1, startMotor1, startMaxMotor1);
MotorTask motorTask2(motor2, stopMotor2, dirMotor2, aVel2, steps2, startMotor2, startMaxMotor2);

// Create scan board task object
ScanBoardTask scanTask;

/* Define tasks for FreeRTOS */
void defMotorTask1(void *p_params)
{
  while (true)
  {
    motorTask1.run();
    vTaskDelay(100); // Task period
  }
}

void defMotorTask2(void *p_params)
{
  while (true)
  {
    motorTask2.run();
    vTaskDelay(100); // Task period
  }
}

/* Main controller task (FSM) */
void defControllerTask(void *p_params)
{
  while (true)
  {
    mainController.run();
    vTaskDelay(100); // Task period
  }
}

void defScanTask(void *p_params)
{
  while (true)
  {
    scanTask.run();
    vTaskDelay(100); // Task period
  }
}

void defKillTask(void *p_params)
{
  while (true)
  {
    if ((digitalRead(XLIM_PIN) == 0) || (digitalRead(YLIM_PIN) == 0))
    {
      stopMotor1.put(true);
      stopMotor2.put(true);
    }
    else
    {
      stopMotor1.put(false);
      stopMotor2.put(false);
    }
    vTaskDelay(100); // Task period
  }
}
/* End of task definitions for FreeRTOS tasks*/

/* Setup and begin multitasking */

/**
 * @brief Main program that sets up FreeRTOS tasks and starts the scheduler
 *
 */
void setup()
{
  Serial.begin(115200); // Begin serial monitor
  while (!Serial)
  {
  } // Wait for port to be ready before continuing

  // Setup pins
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(XLIM_PIN, INPUT_PULLUP);
  pinMode(YLIM_PIN, INPUT_PULLUP);

  xTaskCreate(defMotorTask1, "Motor 1 Task", 2048, NULL, 1, NULL);
  xTaskCreate(defMotorTask2, "Motor 2 Task", 2048, NULL, 1, NULL);
  xTaskCreate(defKillTask, "Kill Task", 2048, NULL, 1, NULL);
  xTaskCreate(defControllerTask, "Controller Task", 4096, NULL, 2, NULL);
  xTaskCreate(defScanTask, "Scan Task", 4096, NULL, 2, NULL);
}
void loop()
{
delay(60000);
};