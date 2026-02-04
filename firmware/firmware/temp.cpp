#include "temp.h"
#include "globals.h" // Assumed to contain mqttClient, TOPIC_TEMPERATURE
#include <DHT.h>
#include <Arduino.h> // For millis() and Serial functions
#include "web.h"
#include "heater.h"
// --- Configuration ---
// The DHT sensor is often connected to GPIO 2 on ESP32/Arduino boards.
int DHTPIN = 2; 
    float targetTemp = 40;  //Maximum temp

float gap = 0.5;
#define DHTTYPE DHT11
// --- End Configuration ---

// Initialize DHT sensor instance
DHT dht(DHTPIN, DHTTYPE);
// Read Temperature (Celsius by default)
float t = dht.readTemperature();
// Timing variable for the sensor reading interval
const long interval = 2000; // 2 seconds
static unsigned long previousMillis = 0; 

// ------------------------------------------
// HELPER FUNCTION IMPLEMENTATION (Required here if not in a separate file)
// ------------------------------------------
// Handles connection check and publishes the provided payload to the given topic.
/*
void publishMqttMessage(const char* topic, const char* payload) {
    if (mqttClient.connected()) {
        mqttClient.publish(topic, payload);
        Serial.printf("Published: %s -> %s\n", topic, payload);
    } else {
        Serial.println("WARNING: MQTT not connected, skipping publish.");
    }
}*/
// ------------------------------------------

void setupTempSensor() {
    dht.begin();
    Serial.println("DHT sensor initialized");
}


void publishSensorData() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        float t = NAN;

        // --- FIXED: Implement simple retry logic using 'goto' based on isnan() ---
        // This relies on the standard DHT library behavior: returns NaN on failure.
        flag: ;  
        
        // 1. Read the temperature value
        t = dht.readTemperature(); 

        // 2. Check if the read operation failed (is NaN)
        if (isnan(t)) {
            Serial.println("WARN: DHT read failed (NaN detected). Retrying immediately.");
            goto flag; 
        }

        // --- END GOTO LOGIC ---

        // --- LOGIC: Store and Publish successful reading ---
        // FIX: lastTemperature is now correctly declared and used here
       float lastTemperature = t; 
        
        

        String tempString = String(lastTemperature, 1); 
        
       
        publishMqttMessage(TOPIC_TEMPERATURE, tempString.c_str()); 
    }
}

// Control heater based on the temperature
void controlHeater() {
    // FIX 2: Added hysteresis (gap) so it doesn't flicker at exactly 50 degrees
    if (t < (targetTemp - gap)) {
        turnHeaterOn();
        Serial.printf("Heater ON: Current Temp %.1f is below Target %.1f\n", t, targetTemp);
    } 
    else if (t > targetTemp) { 
        turnHeaterOff();
        Serial.printf("Heater OFF: Current Temp %.1f reached Target %.1f\n", t, targetTemp);
    }
}
