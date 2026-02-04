#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

// Motor control pin assignments
extern int in1Pin;       // Motor  channel 1 pin
extern int in2Pin;       // Motor  channel 2 pin
extern int enable1Pin;   // Motor  enable pin (PWM pin)
extern int channel;   // PWM channel

// Motor speed control variables
extern boolean rotationDir;  // Variable to save the motor's rotation direction
extern int rotationSpeed;    // Variable to save the motor's rotation speed

// Function declarations
void setupMotor();            // Initialize motor control pins and PWM
void Forward();      // Move motor Forward 
void stopMotor();             // Stop the motor
void Backward();     // Move motor Backward 
void driveMotor(boolean dir, int spd); // Control the motor's direction and speed

#endif // MOTOR_CONTROL_H
