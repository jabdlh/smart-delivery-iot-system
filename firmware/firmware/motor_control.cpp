#include "motor_control.h"

int in1Pin = 12;      // Define  channel 1 pin
int in2Pin = 14;      // Define  channel 2 pin
int enable1Pin = 13;  // Define  enable 1 pin
int channel = 0;  

boolean rotationDir;  // Define a variable to save the motor's rotation direction
int rotationSpeed = 889;    // Define a variable to save the motor rotation speed

void setupMotor() {
  // Initialize the pin into an output mode:
  stopMotor();
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  ledcAttachChannel(enable1Pin,1000,11,channel);//Set PWM to 11 bits, range is 0-2047
  
}

// these functions work in reverse
void Backward() {
  Serial.print("Moving Backward : ");
  driveMotor(false,rotationSpeed);
}
void stopMotor() {
    digitalWrite(in1Pin, LOW); 
  digitalWrite(in2Pin, LOW); 
  digitalWrite(enable1Pin, LOW); 
  Serial.println("Motors stopped");
  driveMotor(false, 0);  // Call driveMotor with speed 0 to stop the motor
  
}

void Forward() {
  Serial.print("Moving Forward :");
    driveMotor(true, rotationSpeed);
}

void driveMotor(boolean dir, int spd) {
  // Control motor rotation direction
  if (dir) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  }
  else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  }
  // Control motor rotation speed
  ledcWrite(enable1Pin, spd);
}
