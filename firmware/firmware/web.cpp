#include "web.h"
#include "globals.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "camera_module.h" 
#include "motor_control.h"
#include <Arduino.h>
#include "lock_control.h"
#include "heater.h"
#include "weight_control.h"
#include "temp.h"
const long interval = 1900; //  seconds
static unsigned long previousMillis = 0;

void setupWiFi() {
    Serial.printf("Connecting to WiFi '%s' ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(300);
    }
    Serial.println("\nWiFi connected");
}

void setupMQTT() {
     tlsClient.setCACert(ca_cert);    // Set secure connection to MQTT
    mqttClient.setClient(tlsClient);  // Set Wi-Fi client for MQTT
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);  // Set MQTT server
    mqttClient.setCallback(mqttCallback);  // Set MQTT callback
    reconnectMQTT();
}

// In web.cpp

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("Connecting MQTT...");
        
        // Define LWT parameters:
        const char* LWT_TOPIC = TOPIC_STATUS;
        const char* LWT_MESSAGE = "OFFLINE";
        const bool LWT_RETAIN = true;
        const int LWT_QOS = 0; // Or 1 if reliability is critical

        // Use the connect function with LWT parameters:
        if (mqttClient.connect(
            CLIENT_ID, 
            MQTT_USER, 
            MQTT_PASS,
            LWT_TOPIC,        // Will Topic
            LWT_QOS,          // Will QoS
            LWT_RETAIN,       // Will Retain
            LWT_MESSAGE       // Will Message
        )) {
            Serial.println("connected");
            mqttClient.subscribe(TOPIC_CONTROL);// Subscribe to control topic
            
            // 1. Publish initial status as ONLINE
         publishMqttMessage(TOPIC_STATUS, "ONLINE", true);
            
        } else {
            Serial.printf("failed rc=%d\n", mqttClient.state());
            delay(2000);
        }
    }
}
//The Listener:react to incoming commands and change the state of the hardware
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length);
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.printf("MQTT %s: %s\n", topic, msg.c_str());
  if (String(topic) == TOPIC_CONTROL) {

if( msg == "CAPTURE"){ captureAndPublish(); } 
               
       else if (msg == "CONVEYOR ON") {

      Forward();  // Call the motor Forward function
      
    
   
      //delay(4000);
      
    } else if (msg == "CONVEYOR OFF") {
    stopMotor();  // Call the motor reverse function
    } else if (msg == "OPEN DOOR") {
      openDoor();
    }          
 else if (msg == "CLOSE DOOR") {
       closeDoor();
    }  

 
      
     else if (msg == "HEATER ON") {
      turnHeaterOn();
        controlHeater();
      
    }  
     else if (msg == "HEATER OFF") {
    turnHeaterOff();
    }  

    else if (msg == "ACCEPT"){
          Forward();
          delay(1650);
          stopMotor();
          closeDoor();
           
           turnHeaterOn();
    }
    else if (msg == "RETRIEVE"){
          Backward();
          delay(1600);
          stopMotor(); 
          turnHeaterOff();
          openDoor(); 
           delay(5000);
           closeDoor();
          unsigned long currentMillis = millis();

    // Check if the publish interval has passed
    
           
            // delay(10000);
            if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
             
    }}
    else if (msg == "REJECT"){
      stopMotor(); 
       openDoor();
        turnHeaterOff();
        delay(5000);
        closeDoor();
        
    }
}
}
// The Speaker:broadcast sensor readings or status update 
// Update function definition to handle the retain flag
void publishMqttMessage(const char* topic, const char* payload, bool retain) {
    if (mqttClient.connected()) {
        mqttClient.publish(topic, payload, retain); // Pass the retain flag here
        Serial.printf("MQTT Published: %s -> %s (Retain: %s)\n", topic, payload, retain ? "YES" : "NO");
    } else {
        Serial.println("MQTT Client not connected, cannot publish.");
    }
}
// send status to the dart app
void publishStatus(const char* statusMessage) {
    publishMqttMessage(TOPIC_STATUS, statusMessage);
}




