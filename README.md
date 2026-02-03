# Adafruit APDS9999 Library

This is the Adafruit APDS9999 Digital Proximity and RGB Sensor library for Arduino.

Tested and works great with the Adafruit APDS9999 Breakout Board
* https://www.adafruit.com/products/TBD

This chip uses I2C to communicate, 2 pins are required to interface. An optional interrupt pin can be used for threshold-based alerts.

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

## Features

* **Proximity sensing** - 11-bit resolution, configurable LED current/pulses/frequency
* **RGB + IR light sensing** - 20-bit resolution, configurable gain and integration time
* **Interrupt support** - Threshold-based interrupts for both proximity and light sensors
* **Variance mode** - Detect relative light changes instead of absolute levels
* **Low power** - Sleep-after-interrupt modes for battery-powered applications

## Installation

To install, use the Arduino Library Manager and search for "Adafruit APDS9999" and install the library.

## Dependencies

* [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)

## Examples

* **fulltest** - Complete test of all sensor features
* **proximity_sensor** - Simple proximity reading example
* **color_sensor** - RGB color sensing example  
* **threshold_interrupt** - Interrupt-based threshold detection

## Usage

```cpp
#include "Adafruit_APDS9999.h"

Adafruit_APDS9999 apds;

void setup() {
  Serial.begin(115200);
  
  if (!apds.begin()) {
    Serial.println("Failed to find APDS9999");
    while (1);
  }
  
  apds.enableProximitySensor(true);
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
}

void loop() {
  uint8_t status = apds.getMainStatus();
  
  if (status & APDS9999_STATUS_PROX_DATA) {
    uint16_t prox;
    if (apds.readProximity(&prox)) {
      Serial.print("Proximity: ");
      Serial.println(prox);
    }
  }
  
  if (status & APDS9999_STATUS_LIGHT_DATA) {
    uint32_t r, g, b, ir;
    if (apds.getRGBIRData(&r, &g, &b, &ir)) {
      Serial.print("R: "); Serial.print(r);
      Serial.print(" G: "); Serial.print(g);
      Serial.print(" B: "); Serial.print(b);
      Serial.print(" IR: "); Serial.println(ir);
    }
  }
  
  delay(100);
}
```

Written by Limor 'ladyada' Fried with assistance from Claude Code for Adafruit Industries.
MIT license, check LICENSE for more information.
All text above must be included in any redistribution.
