/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */
#include "heater.h"
#include <Arduino.h>
#include "temp.h"

// Heater control pin (define the pin connected to the heater)
int heaterPin = 33;  // You can change this based on your wiring


// Setup heater pin
void setupHeater() {
  pinMode(heaterPin, OUTPUT);  // Set heater control pin as output
  digitalWrite(heaterPin, LOW);  // Initially turn off the heater
}



// Turn the heater ON
void turnHeaterOn() {
  digitalWrite(heaterPin, HIGH);  // Turn on the heater
  Serial.println("Heater is ON");
   controlHeater();
}

// Turn the heater OFF
void turnHeaterOff() {
  digitalWrite(heaterPin, LOW);  // Turn off the heater
  Serial.println("Heater is OFF");
}
