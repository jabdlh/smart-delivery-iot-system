#include "globals.h"
#include "camera_module.h"
#include "temp.h"
#include "web.h"
#include <Arduino.h>
#include "motor_control.h"
#include "lock_control.h"
#include "heater.h"
#include "weight_control.h"
const long interval = 1250; // seconds
static unsigned long previousMillis = 0;
// Setup function runs once at startup
void setup() {
   stopMotor();
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  delay(100);

  // Initialize DHT sensor
 setupTempSensor();
  delay(200);

// Initialize Motors
  setupMotor();
 

 // Initialize Heater
setupHeater();
 // Initialize Lock
setupLockControl();

// Initialize the camera
  setupCamera();
  
   // Initialize the weight sensor
   setupHX711();

  // Initialize Wi-Fi
  setupWiFi();

  // Initialize MQTT
  setupMQTT();
 
}

// Loop function runs continuously
void loop() {
  // Handle MQTT connections and messages
  if (!mqttClient.connected()) {
    reconnectMQTT();  // Reconnect if disconnected
  }
 
  mqttClient.loop();
   unsigned long currentMillis = millis();

    // Check if the publish interval has passed
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
      printWeight();
    checkWeight();
   
   //Publish sensor data (Temperature ) periodically
   publishSensorData();
  

    }

}
