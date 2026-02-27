## Academic Challenge Context

This project was developed as part of an academic robotics challenge with the following core requirements:

- 4-wheel vehicle (Ackermann or differential drive)
- At least one PID-controlled system (velocity or orientation)
- Modifiable velocity reference
- Autonomous square trajectory (X meters per side, 5 m < X < 20 m)
- 10-second stop at each waypoint
- Return to starting position
- Telemetry transmission to an external system
- Battery-powered operation
- Clean mechanical presentation

---

## Project Approach

Although the challenge did not require designing the vehicle from scratch, our team decided to build the car completely from zero for learning purposes.

This included:

- Mechanical chassis design and assembly
- Motor and driver integration
- Sensor architecture definition
- Multi-microcontroller firmware development
- Inter-board communication
- Control system implementation

The objective was to understand the system at a full engineering level rather than relying on pre-built platforms.

---

## Repository Structure

CAR-CONTROLLER/
│
├── docs/
├── firmware/
├── hardware/
├── .gitignore
└── README.md


### docs/

Project documentation organized by development phase:

- research → references and background study  
- design → system architecture and decisions  
- testing → validation results  
- reports → academic deliverables  
- media → images and videos  
- development-history → progress tracking  

---

### firmware/

Embedded software for each microcontroller:

- esp32 → high-level control, navigation logic, telemetry  
- stm32 → magnetometer acquisition and sensor interface  

Each subfolder contains board-specific configuration and build instructions.

---

### hardware/

Physical system design files:

- cad → mechanical design files  
- pcb → electronic board layouts  

---

## System Summary

The final system integrates:

- Sensor acquisition (STM32)
- Control and navigation logic (ESP32)
- PID control implementation
- Autonomous trajectory tracking
- Telemetry transmission
- Fully battery-powered operation

The project fulfills the academic requirements while extending the implementation to a complete custom-built embedded system.