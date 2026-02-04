/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */
 #ifndef HEATER_H
#define HEATER_H

// Heater control pin
extern int heaterPin;  // Pin to control the heater

// Function declarations
void setupHeater();
//void controlHeater();
void turnHeaterOn();
void turnHeaterOff();

#endif // HEATER_H
