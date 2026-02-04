/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */
#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Function declarations
void setupWiFi();
void setupMQTT();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
// Update prototype to include optional retain flag
void publishMqttMessage(const char* topic, const char* payload, bool retain = false);
void publishStatus(const char* statusMessage);
#endif // WEB_H
