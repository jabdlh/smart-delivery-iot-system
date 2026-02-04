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
