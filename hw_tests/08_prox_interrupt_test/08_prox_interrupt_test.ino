/*!
 * @file 08_prox_interrupt_test.ino
 * @brief Test proximity interrupt with thresholds
 * @note Connect INT pin to D2
 */

#include <Adafruit_APDS9999.h>

#define INT_PIN 2

Adafruit_APDS9999 apds;

volatile bool intFired = false;

void isr() {
  intFired = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Proximity Interrupt Test");
  Serial.println("----------------------------------");
  Serial.println("Connect sensor INT pin to D2");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable proximity sensor
  apds.enableProximitySensor(true);

  // Set thresholds: interrupt when prox > 500 or prox < 100
  apds.setProxThresholdLow(100);
  apds.setProxThresholdHigh(500);

  // Set persistence (4 consecutive readings required)
  apds.setProxPersistence(4);

  // Enable proximity interrupt
  apds.enableProxInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending interrupt
  apds.getMainStatus();

  Serial.println("\nThresholds set: Low=100, High=500");
  Serial.println("Wave hand near sensor to trigger interrupt!\n");
}

void loop() {
  uint16_t prox;
  apds.readProximity(&prox);

  Serial.print("Proximity: ");
  Serial.print(prox);

  if (intFired) {
    intFired = false;

    // Read status to clear interrupt
    uint8_t status = apds.getMainStatus();

    Serial.print("  *** INTERRUPT FIRED! *** Status: 0x");
    Serial.print(status, HEX);

    if (prox > 500) {
      Serial.print(" (above high threshold)");
    } else if (prox < 100) {
      Serial.print(" (below low threshold)");
    }
  }

  Serial.println();
  delay(100);
}
