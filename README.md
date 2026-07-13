# Park Sense -- Smart Parking Monitoring System

## Overview

Park Sense is an IoT-based smart parking monitoring system that detects
parking slot occupancy in real time using ESP32 and ultrasonic sensors.
The system transmits occupancy data via MQTT for cloud visualization,
helping users and administrators monitor parking availability
efficiently.

## Features

-   Real-time parking slot occupancy detection
-   ESP32-based wireless communication
-   MQTT-based data transmission
-   Cloud monitoring with ThingSpeak
-   Live dashboard for parking status
-   LED/LCD indication of available and occupied slots
-   Scalable architecture for multiple parking slots

## Hardware Used

-   ESP32 DevKit
-   HC-SR04 Ultrasonic Sensors
-   LCD (I2C)
-   LEDs
-   Servo Motor (optional)
-   Raspberry Pi 4 (MQTT Broker)
-   Power Supply

## Software & Technologies

-   Arduino IDE
-   Embedded C
-   MQTT (Eclipse Mosquitto)
-   Python
-   ThingSpeak
-   HTML/CSS/JavaScript

## System Architecture

``` text
Ultrasonic Sensors
        │
        ▼
     ESP32
   ├── LEDs/LCD
   └── MQTT over Wi-Fi
        │
        ▼
 Raspberry Pi (Mosquitto)
        │
        ▼
   ThingSpeak Cloud
        │
        ▼
  Web Dashboard
```

## Project Structure

``` text
├── Arduino_Code/
├── Dashboard/
├── Images/
├── REPORT/
├── README.md
```

## Getting Started

1.  Upload the ESP32 firmware using Arduino IDE.
2.  Connect the HC-SR04 sensors and LCD.
3.  Configure Wi-Fi and MQTT broker details.
4.  Start the Mosquitto broker on the Raspberry Pi.
5.  Run the dashboard and monitor parking slot status.

## Future Improvements

-   Mobile application
-   License plate recognition
-   AI-based parking prediction
-   Smart city integration

## Author

**Dhruv Yadav**\

