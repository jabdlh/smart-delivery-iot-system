/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */
#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include <Arduino.h>
#include <esp_camera.h>

// Declare the setup and capture functions
void setupCamera();
// Function to turn off the camera
void turnOffCamera();

// Image capture and publish functions
void captureAndPublish();

// Declare functions for publishing images
bool publishImageSingle(const uint8_t* buf, size_t len);
void publishImageChunked(const uint8_t* buf, size_t len);

#endif // CAMERA_MODULE_H
