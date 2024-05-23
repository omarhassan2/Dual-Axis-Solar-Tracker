# 🚀Welcome to Our Dual-Axis Solar Tracker Repository🚀

This repository contains the code and documentation for a Dual-Axis Solar Tracker implemented using the PID (Proportional-Integral-Derivative) controller concept. The project aims to efficiently orient solar panels towards the sun throughout the day, maximizing energy generation.

## Table of Contents
1. [Introduction](#introduction)
2. [Circuit Diagram](#circuit-diagram)
3. [PID Controller with Simulink](#pid-controller)
4. [IOT Systems with Firebase & Flutter App](#iot-systems)
5. [Visualization Data with Python](#visualization-data)
6. [Getting Started](#getting-started)

## Introduction

**Key Features:**
- **Dual-axis tracking:** Our solar tracker adjusts both azimuth and elevation angles to precisely follow the sun's path throughout the day.
- **PID Control:** Leveraging the PID control algorithm, our system continuously fine-tunes the position of solar panels for optimal solar exposure, even under changing environmental conditions.
- **User-friendly interface:** We've designed intuitive controls and monitoring tools to facilitate easy interaction and comprehensive performance analysis.
- **Additionally:** Our project incorporates a data logging system using Google Sheets to store data retrieved from Firebase. A Python script is provided to analyze the data and generate plots showcasing the power output of the solar panels throughout the day.

## Circuit Diagram

The circuit diagram for the Dual-Axis Solar Tracker can be found in the `Schematic` directory. This diagram provides a detailed layout of the electrical connections required to build the solar tracker system.

![Circuit Diagram](/Schematic/Schematic.png)

## PID Controller

The PID Controller is a crucial part of our solar tracker system. It ensures precise control over the movement of the solar panels. The Simulink module used to obtain the PID constants (Kp, Ki, Kd).

- Simulink Model: 
![Simulink Model](/Simulink%20Model/Simulink_Model.png)


## IOT Systems

Our project includes an IOT system that utilizes Firebase for real-time data storage and a Flutter app for monitoring and control.

- Download Flutter App -> 'https://drive.google.com/file/d/1UhSQkoldwgrA8FfKU0UBUzVCbPq2rS_C/view?usp=sharing'
  
![Flutter App](/Flutter_App.jpg)

## Visualization Data

After the system work we take the data from Google-Sheet and graph the total power generation along the day. By using Python Script to plot it.

- Google Sheet:
![Google Sheet](/Scripting/Data.png)

- Output Script:
![Output Script](/Scripting/Output_Script.png)

## Getting Started

To get started with the Dual-Axis Solar Tracker project, follow the steps below:

1. **Clone the Repository:**
   ```bash
   'git clone https://github.com/omarhassan2/Dual-Axis-Solar-Tracker.git'
2. **Explore the Code:** Review the code files to understand the project's implementation.
3. **Simulation:** USE a proteus simulator to simulate the project and test its functionality.
4. **Contribute:** Feel free to contribute to the project by submitting bug fixes, improvements, or new features through pull requests.
