# Car Controller – Firmware Layer

## Overview

This directory contains all embedded firmware used in the Car Controller system.

The system is composed of two microcontrollers:

- ESP32 → High-level control and communication
- STM32 → Magnetometer acquisition and sensor interface

The firmware layer is divided by board type to clearly separate responsibilities.

---

## Architecture Overview

The system follows a distributed architecture:

ESP32
  ├─ Bluetooth gamepad interface
  ├─ Manual / Autonomous control logic
  ├─ Motor and servo control
  ├─ Ultrasonic sensing
  ├─ WiFi + MQTT telemetry
  └─ Receives heading data from STM32 via UART

STM32
  ├─ I2C communication with magnetometer
  ├─ Raw magnetic field acquisition (X, Y, Z)
  ├─ Offset calibration
  ├─ ASCII data formatting
  └─ Transmits heading data via USB CDC and UART

The STM32 acts as a dedicated sensor node.  
The ESP32 acts as the system controller.

---

## Folder Structure
firmware/
├── esp32/
│ └── Car-Controller-ESP32/
│
└── stm32/
└── Car-Controller-STM32/

Each subfolder contains:

- Source code
- Board configuration
- Peripheral setup
- Individual README

---

## Communication Flow

1. STM32 reads magnetometer via I2C.
2. STM32 applies calibration.
3. STM32 sends formatted X, Y, Z values via UART.
4. ESP32 receives heading data.
5. ESP32 uses heading for autonomous navigation.
6. ESP32 optionally sends telemetry to cloud.

---

## Design Philosophy

The firmware is divided by responsibility:

STM32 → Deterministic sensor acquisition  
ESP32 → Behavioral control and communication  

This separation improves:

- Modularity
- Debugging
- Scalability
- System clarity

---

## Development Tools

ESP32:
- Arduino IDE
- ESP32 board package
- Bluepad32
- Ubidots MQTT library

STM32:
- STM32CubeIDE
- STM32CubeMX
- HAL drivers

---

## System Capabilities

- Manual driving via Bluetooth gamepad
- Autonomous navigation using magnetometer + ultrasonic
- Encoder-based displacement measurement
- USB and UART debugging
- Cloud telemetry (optional)

---

## Future Improvements

- Refactor STM32 into modular sensor driver
- Implement non-blocking communication
- Add checksum or framing protocol between boards
- Replace ASCII with binary packet format
- Introduce PID steering control
- Separate application logic from hardware layer

---

## System Summary

The firmware layer represents the embedded core of the project.

It integrates:

- Sensor acquisition
- Control logic
- Actuation
- Communication
- Telemetry

This modular multi-MCU architecture reflects scalable embedded system design principles.