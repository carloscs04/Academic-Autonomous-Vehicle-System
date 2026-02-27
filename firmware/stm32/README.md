# Car Controller – STM32 Magnetometer Firmware

## Overview

This firmware runs on an STM32F1 microcontroller and performs:

- I2C communication with a 3-axis magnetometer
- Raw magnetic field acquisition (X, Y, Z)
- Offset compensation (manual calibration gaps)
- ASCII formatting of axis data
- Data transmission via:
  - USB CDC (Virtual COM Port)
  - UART2 @ 115200 baud

The STM32 acts as a **sensor acquisition and communication layer** for the system.

---

## System Architecture Role

This firmware represents the **sensor platform layer**.

It:

1. Initializes system clocks
2. Configures I2C
3. Configures UART
4. Configures USB CDC
5. Reads magnetometer registers
6. Applies calibration offsets
7. Sends formatted data to host (ESP32 or PC)

---

## Hardware Configuration

### MCU
- Family: STM32F1xx
- Clock Source: HSE + PLL (x6)
- USB Clock: PLL
- Flash Latency: 1

---

## Peripheral Configuration

### I2C1
- Speed: 100 kHz
- Addressing Mode: 7-bit
- Used to communicate with magnetometer
- Device address: `0x1A`

Magnetometer configuration registers:
- Register `0x0B`
- Register `0x09`

---

### USART2
- Baud Rate: 115200
- 8 data bits
- 1 stop bit
- No parity
- Used for serial debugging and external communication

---

### USB Device (CDC)

- Virtual COM port interface
- Data transmitted using:

```c
CDC_Transmit_FS(mensaje, 18);