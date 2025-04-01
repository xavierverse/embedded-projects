# ESP32C3 SHTC3 Temperature and Humidity Sensor Lab

This project demonstrates how to interface with the SHTC3 temperature and humidity sensor on the ESP32C3 Rust board using the I2C protocol. The program reads the temperature and humidity values from the sensor every 2 seconds, converting the temperature from Celsius to Fahrenheit and displaying both values along with the humidity percentage.

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation and Setup](#installation-and-setup)
- [Usage](#usage)

## Overview

This project uses the SHTC3 temperature and humidity sensor embedded on the ESP32C3 Rust board to read environmental data every 2 seconds. The sensor communicates with the microcontroller via the I2C protocol. The program outputs the temperature in both Celsius and Fahrenheit, as well as the humidity as a percentage, printed to the serial console.

### Example Output:
Temperature is 20C (or 68F) with a 40% humidity Temperature is 22C (or 72F) with a 42% humidity Temperature is 20C (or 68F) with a 44% humidity

markdown
Copy

## Features

- Reads temperature and humidity from the SHTC3 sensor every 2 seconds.
- Displays the temperature in both Celsius and Fahrenheit.
- Displays the humidity as a percentage.
- Utilizes the I2C protocol for communication with the sensor.

## Hardware Requirements

- **ESP32C3 Rust board** (with an integrated SHTC3 sensor)
- **USB cable** to connect the ESP32C3 to your computer

## Software Requirements

- **IDE**: PlatformIO or any C/C++ compatible IDE that supports ESP32 development.
- **Libraries**:
  - `Wire` library for I2C communication (for PlatformIO, it’s usually included automatically in ESP32 framework)
  - `SHTC3` sensor driver library (you may need to install this library for PlatformIO or use a custom driver)

## Installation and Setup

Follow these steps to get the project up and running:

1. **Clone the repository:**
   ```bash
   git clone https://github.com/xavierverse/esp32c3-shtc3-lab.git
Set up PlatformIO:

Open PlatformIO (or your IDE of choice) and create a new project for the ESP32C3 board.

In your platformio.ini configuration file, ensure the ESP32 platform is set up. It should look something like this:

[env:esp32c3]
platform = espressif32
board = esp32c3
framework = arduino
Install the required libraries:

In PlatformIO, open the platformio.ini file and add the required libraries for I2C communication and the SHTC3 sensor.

Example:

ini
Copy
lib_deps =
    Wire
    SHTC3
Upload the code:

Ensure that the main.c file is set up for your project.

## Usage
Once the program is uploaded to the ESP32C3, open the Serial Monitor in PlatformIO or your IDE. You should see the temperature and humidity values printed every 2 seconds in the following format:

Temperature is 20C (or 68F) with a 40% humidity
Temperature is 22C (or 72F) with a 42% humidity
Temperature is 20C (or 68F) with a 44% humidity