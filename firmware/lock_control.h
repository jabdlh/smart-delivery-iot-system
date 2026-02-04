/**
 * Project: Smart Multi-Compartment Grocery Delivery System
 * Author: Ahmed Alasiri
 * Institution: King Khalid University (2026)
 * Description: IoT-based logistics hub featuring climate control and automated transport.
 */
#ifndef LOCK_CONTROL_H
#define LOCK_CONTROL_H


#include "Relay.h"  // Include the Relay library

// Function declarations for door control
void openDoor();     // Function to open the door (activate the relay)
void closeDoor();
void setupLockControl();    // Function to close the door (deactivate the relay)

#endif // LOCK_CONTROL_H
