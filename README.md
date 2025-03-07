# Weather Station on STM32

This project uses STM32L4 to control the process of performing indoor measurements (temperature, humidity and pressure) and obtaining various weather data through WiFi module and the API.

## Table of Contents
* [About](#about)
* [Software Used](#software-used)
* [Hardware Used](#hardware-used)
* [Hardware Connections](#hardware-connections)
* [Installation](#installation)
* [Project Status](#project-status)

## About
This project is designed as a battery-powered system. It utilizes the RTC module to periodically wake up the STM32 from low-power mode, allowing it to perform measurements and obtain weather data via the ESP8266 (which also makes use of its low-power mode) using the OpenWeatherMap API.

The retrieved data is displayed on a 1.3″ OLED screen. A user button allows switching between different displayed information.

The project is mainly motivated by educational purposes, but with future potential improvements it can become practical and useful device.

## Software Used
- STM32Cube
- Visual Studio Code
- Logic (Saleae)

Libraries, frameworks, APIs:
- STM32CubeHAL
- Arduino for ESP8266 (PlatformIO)
- OpenWeatherMap API

## Hardware Used
- STM32L476RG (Nucleo-L476RG)
- ESP8266 (NodeMCU-V3, ESP-01S)
- Logic Analyzer
- DHT22 (temperature/humidity sensor)
- LPS25HB (pressure sensor)
- 1.3″ OLED display (SH1106 driver)
- Wires, Resistors, Capacitors

## Hardware Connections
During the prototype phase, development boards are powered via USB ports. Therefore, all components and MCUs are powered through the onboard voltage regulators.

| STM32L4 | ESP8266 | Notes                                          |
|---------|---------|------------------------------------------------|
| PA10    | GPIO2   | ESP8266 UART TX1, Add 10kΩ pull-up resistor    |
| PB8     | GPIO3   | Request to ESP8266, Add 100kΩ pull-up resistor |
| PC7     | RST     | Reset ESP8266                                  |
| GND     | GND     | Common Ground                                  |

| STM32L4 | DHT22 | Notes                                                                 |
|---------|-------|-----------------------------------------------------------------------|
| PA6     | DATA  | Data signal, Add 5kΩ pull-up resistor                                 |
| -       | VDD   | Connect to 3V3 power supply, Add 100nF capacitor between VDD and GND  |
| GND     | GND   | Common Ground                                                         |

| STM32L4 | LPS25HB     | Notes           |
|---------|-------------|-----------------|
| PC3     | SDA/SDI/SDO | SPI MOSI        |
| PB10    | SCL/SPC     | SPI Clock       |
| PC2     | SDO/SA0     | SPI MISO        |
| PB11    | CS          | SPI Chip Select |
| GND     | GND         | Common Ground   |

| STM32L4 | Display (SH1106 driver) | Notes                      |
|---------|-------------------------|----------------------------|
| PC3     | SDA                     | I2C SDA                    |
| PB10    | SCK                     | I2C SCK                    |
| -       | VCC                     | Connect to 5V power supply |
| GND     | GND                     | Common Ground              |

## Installation
1. Flash the ESP8266 firmware:  
   - The STM32 communicates with the ESP8266, which requires a separate firmware.  
   - Follow the instructions in the [ESP8266 repository](https://github.com/Romwierz/esp8266-weather-station-wifi-module) to flash the appropriate code. 
2. Clone this repository using Git command:
    ```bash
        git clone https://github.com/Romwierz/weather-station.git
    ```
- Alternatively, download the ZIP and extract it.
3. Open **STM32CubeIDE** and import the project into the workspace:
    - Go to **File** → **Import...** → **General** → **Existing Projects into Workspace**.
    - Select the root directory (the extracted/cloned project folder).
    - Enable the **Copy projects into workspace option**.
4. Open the `.ioc` file and generate the code using the **Device Configuration Tool Code** from the toolbar.
5. Build and run the project.

## Project Status
✅ Core Features Implemented:
- Indoor measurements (temperature, humidity, pressure)
- Weather data retrieval via ESP8266
- OLED display
- Periodic system wake up from sleep to perform tasks

🚀 Potential Upgrades:
- Optimize energy consumption:
    - Use RTC alarms to wake up the ESP8266 before the STM32, ensuring WiFi data is ready without delay
- Display weather forecasts
- Implement motion detection for brightness control
- Create a web interface for WiFi location settings