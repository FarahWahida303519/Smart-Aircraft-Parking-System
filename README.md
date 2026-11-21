# ✈️ Smart Aircraft Parking Bay Monitoring System

##  Project Overview

The **Smart Aircraft Parking Bay Monitoring System** is an ESP32-based project developed to monitor a model aircraft parking bay.
The system uses two sensors to collect information and displays the results on an OLED screen. Users also can switch between two display pages using a capacitive touch pin.

## Project Objectives

- Measure the distance of an approaching object.
- Detect whether the parking bay is occupied or available.
- Display real-time sensor information on an OLED screen.
- Use capacitive touch to switch between two display pages.

## Hardware Components

| Component | Purpose |
|---|---|
| ESP32 30-pin | Main microcontroller |
| Custom ESP32 Breakout Board | Connects the ESP32 to external components |
| HC-SR04 Ultrasonic Sensor | Measures object distance |
| FC-51 IR Obstacle Sensor | Detects object presence |
| SSD1306 OLED Display | Displays sensor information |
| Male Pin / Jumper Wire | Capacitive touch input |
| 1 kΩ Resistors (3 units) | Voltage divider for HC-SR04 ECHO |

##  GPIO Pin Connections

| Component | Pin | ESP32 Connection |
|---|---|---|
| **HC-SR04 Ultrasonic Sensor** | VCC | VIN (5V) |
| | GND | GND |
| | TRIG | GPIO 5 |
| | ECHO | GPIO 18 (through voltage divider) |
| **FC-51 IR Obstacle Sensor** | VCC | 3V3 |
| | GND | GND |
| | OUT | GPIO 19 |
| **OLED Display (SSD1306)** | VCC | 3V3 |
| | GND | GND |
| | SDA | GPIO 21 |
| | SCL | GPIO 22 |
| **Capacitive Touch** | Male Pin | GPIO 27 |


## System Features

**1. Parking Bay Status**

The FC-51 sensor detects an object and updates the parking status:
- OCCUPIED – Object detected.
- AVAILABLE – No object detected.

**2. Aircraft Distance Monitoring**

The HC-SR04 measures the distance of an approaching object and displays the result in centimetres (cm).

**3. OLED Display**

The OLED provides two display pages:
- Page 1 – Parking Bay Status.
- Page 2 – Aircraft Monitoring.

**4. Capacitive Touch Navigation**

Users can touch and hold the male pin connected to GPIO27 for three seconds to switch between the two OLED pages.

