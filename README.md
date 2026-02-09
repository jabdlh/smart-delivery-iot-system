
                       

   **Smart Multi-Compartment Grocery Delivery System**        


## **image of the prototype**


![Project Overview](prototype_image.png)


**Project Overview**
This project is an advanced IoT Logistics Hub designed for secure, climate-controlled package delivery. It integrates an ESP32 central controller with a series of sensors and actuators to automate item handling, security, and environmental monitoring. The system is managed remotely via a dedicated Flutter/Dart mobile application.

**System Features & Logic**
Climate-Controlled Storage: Maintains internal conditions using a Heater and Temperature Sensor to ensure groceries or medicine remain at optimal temperatures.

Automated Logistics: Features a Conveyor Belt and Motor system for the contactless transport of items within the unit.

Intelligent Verification: Uses the Weight Sensor as a trigger mechanism; when an item is detected, the Camera captures an image for immediate user verification.

Enhanced Security: Features an Electromagnetic Lock for high-strength physical security, controllable via the mobile interface.

Real-Time Remote Access: Full telemetry and control provided by a Mobile App built with Flutter, communicating over WiFi/MQTT.

## **Lead Responsibilities**
As the **Technical Project Lead**, I oversaw the end-to-end development of the system, with a primary focus on system architecture and hardware-software integration:
* **System Architecture:** Defined the communication protocol (MQTT/WiFi) between the ESP32 and the Flutter mobile app.
* **Integration Oversight:** Managed the interfacing of 12V high-power actuators (Motor/Locks) with 3.3V logic microcontrollers.
* **Technical Quality Control:** Performed all final code reviews, debugging of the sensor fusion logic.

  
**Hardware Architecture (BOM)**

Microcontroller: ESP32 - The "Brain" handling all wireless processing and sensor logic.

Transport: DC Motor & Conveyor Belt.

Thermal: Resistance Heater + Digital Temperature Sensor.

Vision & Load: ESP32-CAM (OV2640 Module) + Load Cell (Weight Sensor) with HX711 Amplifier.

Security: 12V Solenoid Electromagnetic Lock.

Power Management: AC-DC Switch Mode Power Supply (SMPS) – Converts 220V AC (Mains) to 12V DC to power the high-load components (Motor, Heater, Locks), with a buck converter for the 3.3V/5V ESP32 logic.

**Future Enhancements**
Local Interface: Integrating an I2C 16x2 LCD Display to provide real-time status updates (e.g., "Temperature OK", "Package Detected") directly on the unit.

Solar Power: Adding a photovoltaic charging circuit for off-grid deployment.

**Author** :

Ahmed Alasiri Computer Engineering Graduate (2026) King Khalid University
