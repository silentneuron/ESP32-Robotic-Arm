# ESP32 Robotic Arm Project

A low-cost 3-DOF robotic arm powered by the **ESP32**, designed for **web-based control** and **autonomous object pickup** using **OpenCV color detection**.

---

##  Features

- **Web Interface** hosted directly on ESP32  
  - Real-time servo control (base, shoulder, elbow, gripper)  
- **Automatic Object Pickup**
  - Detects **red** and **blue** objects using OpenCV
  - Moves and places objects automatically on a palm or platform
- **Wi-Fi Connectivity**
  - ESP32 connects as an access point or to local Wi-Fi
- **Cross-language integration**
  - C++ (ESP32 control logic)
  - Python (OpenCV for color detection)
- **Low-cost design**
  - Compatible with standard SG90 or MG996R servos

---

##  System Overview

| Component | Description |
|------------|--------------|
| **ESP32** | Handles servo control and web interface |
| **Servo Motors** | 4 DOF: base, shoulder, elbow, gripper |
| **Python + OpenCV** | Detects red/blue boxes and sends commands to ESP32 |
| **WebSocket** | Enables real-time control between web UI and ESP32 |
| **Power Supply** | 5V or 6V depending on type of servos |

---

##  Web Interface

- Hosted directly by ESP32 using **AsyncWebServer**
- Control panel for manual operation  
- Works on phone/laptop browser

---


## Requirements

### **Hardware**
- ESP32 Dev Board  
- 4x Servo Motors (base, shoulder, elbow, gripper)  
- Power supply (≥2A)  
- Jumper wires and arm chassis
- Could buy here https://robu.in/product/pro-range-esp32-robotic-arm-with-web-control-kit/
- Laptop cam or a web cam
  

### **Software**
- Arduino IDE   
- Libraries:
  - `ESPAsyncWebServer`
  - `AsyncTCP`
  - `Servo.h`
- Python 3.x + OpenCV

---

## Setup Guide

1. **Flash ESP32**
   - Upload `esparm2.ino` or `esparm7.ino` (can be used for 6dof but lag might be noticed) code using Arduino IDE.
2. **Connect to Wi-Fi**
   - Update credentials in the code (`ssid`, `password`).
3. **Run Python Program**
   - Start color detection:
     ```bash
     robot_vision_control.py
   -Conect to the required COM port
   -Even `OpenCV code red color tracking.py` can be used for wireless conection lag might be noticed
4. **Control via Browser**
   -Conect to the SSID and write in the password
   - Open: `http://<ESP32 IP from terminal>`  
   - Move arm.

---


##  Future Improvements

- Integrate camera feed directly into web UI  
- Enable gesture-based control using webcam
- Using rasberry pi reduce lag but price increases

---


##  Author

**Silent_neuron**  
_"If it moves, automate it."_ 
