/*!
 * @file 09_light_interrupt_test.ino
 * @brief Test light sensor interrupt with thresholds
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

  Serial.println("APDS9999 Light Interrupt Test");
  Serial.println("-----------------------------");
  Serial.println("Connect sensor INT pin to D2");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable light sensor
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);

  // Set thresholds - these will need adjustment based on ambient light
  // Low threshold = dark, High threshold = bright
  apds.setLightThresholdLow(1000);
  apds.setLightThresholdHigh(50000);

  // Use green channel for interrupt
  apds.setLightIntChannel(APDS9999_LIGHT_INT_CH_GREEN);

  // Set persistence (4 consecutive readings required)
  apds.setLightPersistence(4);

  // Enable light interrupt
  apds.enableLightInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending interrupt
  apds.getMainStatus();

  Serial.println("\nThresholds set: Low=1000, High=50000 (Green channel)");
  Serial.println("Cover/uncover sensor to trigger interrupt!\n");
}

void loop() {
  uint32_t r, g, b, ir;
  apds.getRGBIRData(&r, &g, &b, &ir);

  Serial.print("G: ");
  Serial.print(g);

  if (intFired) {
    intFired = false;

    // Read status to clear interrupt
    uint8_t status = apds.getMainStatus();

    Serial.print("  *** INTERRUPT FIRED! *** Status: 0x");
    Serial.print(status, HEX);

    if (g > 50000) {
      Serial.print(" (above high threshold - bright)");
    } else if (g < 1000) {
      Serial.print(" (below low threshold - dark)");
    }
  }

  Serial.println();
  delay(200);
}
