/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */
#ifndef HX711SETUP_H
#define HX711SETUP_H

#include <HX711_ADC.h>
#include <EEPROM.h>  // Include EEPROM library

// Pin Definitions for HX711
extern const int HX711_dout; // data output pin
extern const int HX711_sck;  // clock pin

// Create the HX711_ADC object
extern HX711_ADC LoadCell;

// Function declarations
void setupHX711();
float readWeight();
void printWeight(); //<<<<<<<< good function 

void checkWeight();




void turnOnOnce();

#endif
