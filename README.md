**Project Title**
CAN-Based Automotive Black Box with Event Logging and User Interface
**Overview**
This project implements a Car Black Box system using two ECUs communicating via CAN protocol.

**ECU1 (Main Unit)**
Receives vehicle data via CAN
Displays information on CLCD
Logs events into EEPROM
Provides password-protected menu
Supports time setting using RTC

**ECU2 (Sensor Unit)**
Reads vehicle parameters (speed, gear, status)
Sends data via CAN

**Features**
- Real-Time Monitoring
  Displays:
  Time (RTC - DS1307)
  Engine status
  Speed (from ADC via ECU2)
- Secure Access
  Password-based login (4-key sequence)
  Lock system after 3 wrong attempts
- Event Logging
  Logs up to 10 events in EEPROM
  Stores:
  Gear, seatbelt, brake, door, engine, collision
  Time (HH:MM:SS)
  Speed
- Collision Detection
  Stops logging after collision

**Displays:**
  COLLISION HAPPENED
  ENGINE OFF
  Menu Options
  View Logs
  Set Time
  
**Hardware Components**
PIC Microcontroller (e.g., PIC18F4580)
MCP2551 CAN Transceiver
16x2 CLCD
Matrix Keypad (4x3)
DS1307 RTC
External EEPROM (24Cxx)
Potentiometer (ADC for speed)

**Communication**
CAN Messages (ECU2 → ECU1)
Data	DLC	Description
Speed	3	ASCII digits
Status	6	Gear + flags

**Status Format:** [Gear][Seat][Brake][Door][Engine][Collision]

**Software Architecture**
  State Machine (ECU1)
  States:
  ST_DEFAULT
  ST_LOGIN
  ST_MENU
  ST_VIEW_LOG
  ST_SET_TIME

**Password** K1 K2 K1 K2

**EEPROM Storage Format**
Each event (13 bytes):
Field	Size
Event ID	1
Gear	1
Seat	1
Brake	1
Door	1
Engine	1
Collision	1
Time (hh mm ss)	3
Speed	3

**Working Principle**
**ECU2:**
Read ADC → Speed
Read keypad → Status
Send via CAN:
Speed (3 bytes)
Status (6 bytes)
**ECU1:**
Receive CAN data
Read RTC time
Store event in EEPROM
Display on LCD
Handle user input (menu system)

**Key Concepts Used**
CAN Protocol (Multi-frame communication)
I2C (RTC + EEPROM)
State Machine Design
Embedded UI (LCD + Keypad)
Event Logging System
**Real-Time Applications**
Automotive Black Box
Fleet Monitoring Systems
Accident Data Recorder
Vehicle Diagnostics
