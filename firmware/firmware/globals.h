#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>  // Include the MQTT client library
// Include the certificate from CERTIFICATE file
extern const char* ca_cert;     // Include the file where CA certificate is stored
// Declare global variables for MQTT topics
extern const char* TOPIC_TEMPERATURE;  // Declare the sensor topic
extern const char* TOPIC_IMAGE;
extern const char* TOPIC_CONTROL;
extern const char* TOPIC_WEIGHT;
extern const char* TOPIC_STATUS;
// Declare global variables for Wi-Fi and MQTT
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASS;
extern const char* CLIENT_ID;

// Camera pin definitions for the WROVER model
extern const int PWDN_GPIO_NUM;
extern const int RESET_GPIO_NUM;
extern const int XCLK_GPIO_NUM;
extern const int SIOD_GPIO_NUM;
extern const int SIOC_GPIO_NUM;
extern const int Y9_GPIO_NUM;
extern const int Y8_GPIO_NUM;
extern const int Y7_GPIO_NUM;
extern const int Y6_GPIO_NUM;
extern const int Y5_GPIO_NUM;
extern const int Y4_GPIO_NUM;
extern const int Y3_GPIO_NUM;
extern const int Y2_GPIO_NUM;
extern const int VSYNC_GPIO_NUM;
extern const int HREF_GPIO_NUM;
extern const int PCLK_GPIO_NUM;



//extern const int CONVEYORPIN;

extern const int LED_PIN;
// Declare the Wi-Fi client and MQTT client globally
extern WiFiClientSecure tlsClient;   // Wi-Fi client for secure connection
extern PubSubClient mqttClient;      // MQTT client


#endif // GLOBALS_H
