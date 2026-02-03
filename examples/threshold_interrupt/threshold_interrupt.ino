/*
 * Threshold interrupt example for APDS-9999
 * Demonstrates proximity and light sensor threshold interrupts
 *
 * Hardware: Connect INT pin to Arduino pin 2 for interrupt
 */

#include "Adafruit_APDS9999.h"

Adafruit_APDS9999 apds;

#define INT_PIN 2
volatile bool interruptFired = false;

void onInterrupt() {
  interruptFired = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("APDS-9999 Threshold Interrupt Demo"));

  if (!apds.begin()) {
    Serial.println(F("Failed to find APDS-9999"));
    while (1)
      delay(10);
  }

  // Enable both sensors
  apds.enableProximitySensor(true);
  apds.enableLightSensor(true);
  apds.setRGBMode(true);

  // === Proximity Threshold Setup ===
  apds.setProxThresholdLow(50);   // Interrupt when prox < 50 (object far)
  apds.setProxThresholdHigh(200); // Interrupt when prox > 200 (object close)
  apds.setProxPersistence(2);     // Require 3 consecutive readings
  apds.enableProxInterrupt(true);

  Serial.println(F("Proximity thresholds: low=50, high=200, persistence=3"));

  // === Light Threshold Setup ===
  apds.setLightThresholdLow(1000);   // Interrupt when light < 1000
  apds.setLightThresholdHigh(50000); // Interrupt when light > 50000
  apds.setLightIntChannel(APDS9999_LIGHT_INT_CH_GREEN); // Use green channel
  apds.setLightPersistence(2); // Require 3 consecutive readings
  apds.enableLightInterrupt(true);

  Serial.println(F("Light thresholds: low=1000, high=50000 (green channel)"));

  // Setup hardware interrupt (optional - can also poll status)
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), onInterrupt, FALLING);

  Serial.println(F("\nWaiting for interrupts..."));
  Serial.println(F("- Wave hand near sensor for proximity interrupt"));
  Serial.println(F("- Cover/uncover sensor for light interrupt\n"));
}

void loop() {
  // Check for interrupt (hardware or poll)
  if (interruptFired) {
    interruptFired = false;

    // Read status once (clears all flags)
    uint8_t status = apds.getMainStatus();

    if (status & APDS9999_STATUS_PROX_INT) {
      uint16_t prox;
      if (apds.readProximity(&prox)) {
        Serial.print(F(">>> PROXIMITY INTERRUPT! Value: "));
        Serial.println(prox);
      }
    }

    if (status & APDS9999_STATUS_LIGHT_INT) {
      uint32_t r, g, b, ir;
      if (apds.getRGBIRData(&r, &g, &b, &ir)) {
        Serial.print(F(">>> LIGHT INTERRUPT! Green: "));
        Serial.println(g);
      }
    }
  }

  // Also print periodic readings
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();

    uint16_t prox;
    uint32_t r, g, b, ir;

    if (apds.readProximity(&prox)) {
      Serial.print(F("Prox: "));
      Serial.print(prox);
    }

    if (apds.getRGBIRData(&r, &g, &b, &ir)) {
      Serial.print(F("\tGreen: "));
      Serial.print(g);
    }

    Serial.println();
  }
}
