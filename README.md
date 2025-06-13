# Smart Office Cubicle Monitoring System

## Overview

This project implements a cost-effective, privacy-respecting system for monitoring cubicle occupancy in a modern office environment.  It uses an ESP32 microcontroller, PIR motion sensor, LED, and buzzer to detect employee presence and absence, logging sitting time and departure information, and displaying it via a locally hosted web interface.

## Features

*   **Non-Intrusive Occupancy Detection:** Uses a PIR motion sensor for presence detection without cameras.
*   **Real-Time Data:** Displays occupancy status, sitting time, and departure information via a web interface.
*   **Visual and Auditory Feedback:** LED indicates occupancy; buzzer signals arrival/departure.
*   **Data Logging:** Tracks sitting time and departure durations.
*   **Local Web Server:** ESP32 hosts a web server for real-time monitoring within the local network.
*   **Privacy-Centric:** No cameras or other invasive monitoring methods are used.
*   **Affordable:** Uses low-cost, readily available components.

## Hardware Components

*   ESP32 Microcontroller
*   PIR Motion Sensor
*   LED
*   Buzzer

## Software

*   Arduino IDE
*   WiFi.h library
*   HTML, CSS for web interface

## Wiring
*  ESP32 connected to a PIR sensor (GPIO 14), LED (GPIO 0), and buzzer (GPIO 22) on a breadboard.

## Usage
1.  Compile and upload the code to your ESP32 using the Arduino IDE.
2.  Connect to the ESP32's Wi-Fi network (SSID: ESP32-Network, Password: Esp32-Password).
3.  Open a web browser and navigate to the ESP32's IP address to view the monitoring data.
4.  The PIR sensor is calibrated for a 5-meter detection range, and the buzzer is tuned for distinct arrival and departure tones.

## Challenges and Solutions
1. PIR Sensor Sensitivity: Initial false positives were mitigated by adjusting
the sensor’s sensitivity and adding a 30-second detection window to filter
transient movements.

2. Wi-Fi Stability: Client disconnections were resolved by implementing a 2-
second timeout and optimizing the web server’s HTTP response handling.

3. Time Precision: The millis() function’s overflow after 49 days was noted,
with a recommendation for a real-time clock (RTC) module in long-term deployments.

## Future Enhancements

*   Cloud integration for centralized data storage and analytics.
*   Multi-node deployment for monitoring multiple cubicles.
*   Machine learning algorithms to predict occupancy trends.

## License

## YASHARTH KESARWANI
