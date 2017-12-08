# Cayenne MQTT ESP Library
The Cayenne MQTT ESP Library provides functions to easily connect to the [Cayenne IoT project builder](https://www.cayenne-mydevices.com). This library is designed to work with ESP8266 and ESP32 WiFi modules. With it you can send data to and receive data from Cayenne.

![alt text](https://cdn-learn.adafruit.com/assets/assets/000/024/792/medium640/adafruit_products_2471_iso_assembled_01_ORIG.jpg?1429908417)

[Adafruit HUZZAH ESP8266 image](https://learn.adafruit.com/assets/24792) by [lady ada](https://learn.adafruit.com/users/adafruit2) is licensed under [CC BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/)

![alt text](https://cdn.sparkfun.com//assets/parts/1/1/5/6/4/13907-01.jpg)

[SparkFun ESP32 Thing image](https://www.sparkfun.com/products/13907) by [SparkFun](https://www.sparkfun.com) is licensed under [CC BY 2.0](https://creativecommons.org/licenses/by/2.0/)

## Requirements
### Hardware
* An ESP8266 or ESP32 module, e.g. the [Adafruit HUZZAH ESP8266](https://www.adafruit.com/product/2471) or [SparkFun ESP32 Thing](https://www.sparkfun.com/products/13907).
* A USB to Serial FTDI or console cable.

### Software
* [Arduino IDE](https://www.arduino.cc/en/Main/Software) for Windows, Linux or Mac OS.
* [This library](https://github.com/myDevicesIoT/Cayenne-MQTT-ESP8266/archive/master.zip).

## Getting Started
### Environment Setup
1. Download and install the [Arduino IDE](https://www.arduino.cc/en/Main/Software).
2. Install the Board Package.
   1. For **ESP8266**:
      1. Under **File -> Preferences** add `http://arduino.esp8266.com/stable/package_esp8266com_index.json` to the **Additional Boards Manager URLs** field.
      2. Install the **esp8266** platform from **Tools -> Board -> Boards Manager**.
   2. For **ESP32**, manually install the board package by following the instructions here: https://github.com/espressif/arduino-esp32/blob/master/README.md#installation-instructions.
3. Download this library as a zip file [here](https://github.com/myDevicesIoT/Cayenne-MQTT-ESP8266/archive/master.zip).
4. Install the downloaded zip library from **Sketch -> Include Library -> Add .ZIP Library**.
5. Connect your ESP module to your computer using a USB to Serial FTDI or console cable.
6. Select your ESP module and port from the **Tools** menu.

### Cayenne Setup
1. Create your Cayenne account at https://www.cayenne-mydevices.com.
2. Add a new device using the Bring Your Own Thing API selection.

### Building Examples
1. Open the included example sketch from **File -> Examples -> Cayenne-MQTT-ESP**.
2. Modify the included sketch with your network info, and the Cayenne authentication info you received when adding your device.
3. Compile and upload the sketch to your device. Uploading may require setting the device into bootload mode. For the Adafruit HUZZAH ESP8266 module this is done with the folowing steps:
   1. Hold down the **GPIO0** button.
   2. Click the **Reset** button.
   3. Let go of the **GPIO0** button.
   4. The red LED should light up, indicating the device is ready for uploading.
   5. Upload your sketch.
4. Check the Cayenne dashboard to ensure it is receiving sample data from your device.

## Compatibility
This library was tested with the Adafruit HUZZAH ESP8266 breakout module and an Espressif ESP32 development board. It should also work with other standard ESP8266 and ESP32 modules that are compatible with the Arduino IDE.

## Contributions
* This library includes the [Eclipse Paho MQTT C/C++ client](https://github.com/eclipse/paho.mqtt.embedded-c).

## Cayenne MQTT Libraries
Additional libraries are available for connecting to Cayenne on other platforms and devices. These can be found at https://github.com/myDevicesIoT.
