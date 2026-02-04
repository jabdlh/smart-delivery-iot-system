#ifndef TEMP_H
#define TEMP_H

#include <Arduino.h>
extern  int DHTPIN ;  

void setupTempSensor();
void publishSensorData();

void controlHeater();


#endif // TEMP_H
