//works in reverse
#include "lock_control.h"

// Define the relay pin (GPIO) for controlling the door (lock)
#define DOOR_PIN  32 // Example pin number for controlling the door (change as needed)

// Create a Relay object. The second argument defines if the relay is Normally Open (true) or Normally Closed (false).
Relay doorControl(DOOR_PIN, true);  // true = Normally Open, false = Normally Closed

void openDoor() {
  //works in reverse
  doorControl.turnOff();  // Deactivate the relay (turn the door on, if relay is active-low)
  Serial.println("Door opened");
}

void closeDoor() {
  doorControl.turnOn();   // Activate the relay (turn the door off, if relay is active-low)
  Serial.println("Door closed");
}

void setupLockControl() {
  doorControl.begin();  // Initialize the relay control pin
  closeDoor();   // Ensure the door is initially closed
}
