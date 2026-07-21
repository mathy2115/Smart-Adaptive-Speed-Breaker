# Smart Adaptive Speed Breaker 🚦

**Sensitive pedestrian areas like school and hospital zones lack an affordable, localized traffic system that actively intercepts violators without unnecessarily delaying compliant drivers or emergency vehicles.**

This repository contains the C++ logic for an offline, localized traffic enforcement prototype tailored for high-pedestrian zones. Utilizing an ESP32 microcontroller and dual ultrasonic sensors, the system calculates vehicle velocity via time-of-flight math and actuates a physical barrier for overspeeding or wrong-way drivers.

## 🌟 Features
* **Adaptive Speed Enforcement:** Actively targets overspeeding vehicles while allowing normal traffic to pass without slowing down.
* **Wrong-Way Detection:** Immediately flags and intercepts vehicles entering from the opposite direction.
* **Scale Prototype Physics:** Tuned to a `0.25 m/s` threshold to mathematically simulate a 57.6 km/hr (approx. 35 mph) municipal speed limit on a 1:64 scale desktop prototype.
* **Real-Time I2C Diagnostics:** Displays operational logs, speed calculations, and violation states on a 16x2 LCD using a 2-wire data bus.

## 🛠️ Hardware Requirements
* **Microcontroller:** ESP32 Development Board (32-bit, 3.3V Logic)
* **Sensors:** 2x HC-SR04 Ultrasonic Distance Sensors
* **Actuator:** MG90S Micro Servo (Metal Gear)
* **Display:** 16x2 LCD with PCF8574 I2C Expander
* **Indicators:** 3x LEDs (Green, Yellow, Red)

## 🔌 Circuit Pinout Table

| Component | Pin Function | ESP32 GPIO Pin |
| :--- | :--- | :--- |
| **Sensor 1 (Entry)** | TRIG1 & ECHO1 | **Pin 5** & **Pin 19** |
| **Sensor 2 (Exit)** | TRIG2 & ECHO2 | **Pin 18** & **Pin 32** |
| **LCD Display (I2C)** | SDA & SCL | **Pin 21** & **Pin 22** |
| **Servo Motor** | PWM Signal | **Pin 13** |
| **Warning Lights** | Green, Yellow, Red | **Pin 25, 26, 27** |

## 🚀 How It Works
1. **Stealth Mode:** The system idles, constantly pinging the entry and exit zones using 40kHz acoustic pulses.
2. **Detection & Math:** If Sensor 1 triggers before Sensor 2, the ESP32 calculates the time difference and derives the velocity ($v = d/t$).
3. **Execution:** If the velocity exceeds the 0.25 m/s threshold, the system triggers the yellow warning LED and actuates the MG90S servo to 90 degrees to block the path. If Sensor 2 triggers first, a wrong-way violation is logged immediately.
