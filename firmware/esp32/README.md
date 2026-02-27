# Car Controller – ESP32 Firmware

## Overview

This firmware runs on an ESP32 and implements a car control system with:

- Bluetooth gamepad control (Bluepad32)
- Manual and autonomous driving modes
- Ultrasonic distance sensing
- Magnetometer-based orientation control (via UART from STM32)
- Quadrature encoder feedback
- Servo steering control
- Motor PWM control
- Optional WiFi + MQTT telemetry using Ubidots

The ESP32 acts as the high-level controller of the vehicle.

---

## Hardware Requirements

- ESP32 Dev Module
- DC motor with driver stage
- Steering servo
- Ultrasonic sensor (HC-SR04 type)
- Quadrature encoder
- External STM32 (magnetometer via UART)
- WiFi network (for telemetry)

---

## Pin Configuration

| Function              | GPIO |
|-----------------------|------|
| Servo                 | 12   |
| Ultrasonic Trigger    | 14   |
| Ultrasonic Echo       | 27   |
| Motor Forward         | 26   |
| Motor Backward        | 25   |
| Motor PWM (LEDC ch2)  | 33   |
| UART2 RX (STM32 TX)   | 16   |
| UART2 TX (STM32 RX)   | 17   |
| Encoder Channel A     | 19   |
| Encoder Channel B     | 18   |

---

## Software Dependencies

Install the following libraries in Arduino IDE:

- Bluepad32
- ESP32Servo
- UbidotsEsp32Mqtt
- ESP32 Board Support Package
- WiFi (included in ESP32 core)

---

## Functional Description

### Bluetooth Control

- Supports up to 4 controllers.
- Reads joystick axes, brake, throttle and buttons.
- Buttons toggle:
  - Automatic mode
  - WiFi telemetry
  - Autonomous start

---

### Manual Mode (Control)

In manual mode:

- Steering controlled by left joystick X-axis.
- Motor direction and speed controlled by throttle and brake.
- Encoder displacement computed.
- Optional telemetry sent to Ubidots.

---

### Autonomous Mode (Automatico)

In automatic mode:

- Ultrasonic sensor measures obstacle distance.
- Magnetometer heading received via UART.
- Proportional steering correction applied.
- Directional state machine:
  - North
  - East
  - South
  - West
- Obstacle avoidance sequence executed when needed.

---

### Sensors

#### Ultrasonic

Distance calculation:

distance = (duration × sound_speed) / 2

Sound speed constant:
0.034 cm/µs

---

#### Encoder

Quadrature decoding using interrupts.

Displacement:

displacement = (pulses / pulses_per_rev) × wheel_perimeter

---

#### Magnetometer

Data received from STM32 via UART in CSV format:

X,Y,Z

Heading calculated using:

atan2(Y, X)

Normalized to 0–360 degrees.

---

### Telemetry (Optional)

When enabled:

- Connects to WiFi
- Publishes to Ubidots:
  - Heading
  - Encoder displacement
  - Ultrasonic distance

Publishing frequency controlled by PUBLISH_FREQUENCY.

---

## Build Instructions

1. Install Arduino IDE
2. Install ESP32 board support
3. Install required libraries
4. Open Car-Controller-ESP32.ino
5. Select correct ESP32 board
6. Upload at 115200 baud

---

## Configuration Notes

- WiFi credentials are hardcoded
- Ubidots token is hardcoded
- For production use, move credentials to a separate configuration file

---

## Architecture Summary

The firmware is organized into:

- Input processing
- State management
- Control logic
- Actuation
- Optional telemetry

The ESP32 handles high-level control and communication.

---

## Known Limitations

- Uses blocking delays
- Credentials stored in plain text
- Proportional control only (no PID)
- Single-loop architecture

---

## Future Improvements

- Replace delay() with non-blocking timing
- Implement full PID control
- Move configuration to external file
- Separate tasks using FreeRTOS
- Refactor state machine for modularity