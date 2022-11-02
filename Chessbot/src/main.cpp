#include <Arduino.h>

#define EN_PIN_1    14 								// LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_1  33 								// Step on rising edge
#define DIR_PIN_1   32    
#define MS_1_1      34
#define MS_2_1      35    
#define EN_PIN_2    13 								// LOW: Driver enabled. HIGH: Driver disabled
#define STEP_PIN_2  25 								// Step on rising edge
#define DIR_PIN_2   26          
#define MS_1_2      6
#define MS_2_2      7

#include "Motor_Driver.h" 								// Step on rising edge

void setup() {
  	Serial.begin(115200);							// Init used serial port
	  while(!Serial);									// Wait for port to be ready

	  // Prepare pins
	  pinMode(EN_PIN_1, OUTPUT);
	  pinMode(STEP_PIN_1, OUTPUT);
    Motor motor_1(EN_PIN_1, STEP_PIN_1, DIR_PIN_1, MS_1_1, MS_2_1);
    Motor motor_2(EN_PIN_2, STEP_PIN_2, DIR_PIN_2, MS_1_2, MS_2_2);
    
    motor_1.enable(true);
    motor_2.enable(true);
    motor_1.Velocity_MAX(1,1000);
    motor_2.Velocity(-300,600);


}

void loop() {
  // put your main code here, to run repeatedly:
}