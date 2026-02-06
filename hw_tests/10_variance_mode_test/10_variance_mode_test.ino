/*!
 * @file 10_variance_mode_test.ino
 * @brief Test light variance interrupt mode
 * @note Connect INT pin to D2
 */

#include <Adafruit_APDS9999.h>

#define INT_PIN 2

Adafruit_APDS9999 apds;

volatile bool intFired = false;
uint32_t intCount = 0;

void isr() {
  intFired = true;
  intCount++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Variance Mode Test");
  Serial.println("---------------------------");
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
  apds.setLightMeasRate(APDS9999_LIGHT_RATE_50MS);

  // Use green channel for interrupt
  apds.setLightIntChannel(APDS9999_LIGHT_INT_CH_GREEN);

  // Enable variance mode with low threshold (sensitive to changes)
  apds.setLightVarianceMode(true);
  apds.setLightVariance(APDS9999_LIGHT_VAR_32); // 32 count variance

  // Set persistence to 1 (respond quickly)
  apds.setLightPersistence(1);

  // Enable light interrupt
  apds.enableLightInterrupt(true);

  // Verify variance mode is enabled
  bool varMode = apds.getLightVarianceMode();
  Serial.print("Variance mode enabled: ");
  Serial.println(varMode ? "YES" : "NO");

  apds9999_light_variance_t var = apds.getLightVariance();
  Serial.print("Variance threshold: ");
  Serial.println(var);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // Clear any pending interrupt
  apds.getMainStatus();

  Serial.println("\nVariance threshold set to 32 counts");
  Serial.println("Wave hand over sensor to create rapid light changes!\n");
}

void loop() {
  uint32_t r, g, b, ir;
  apds.getRGBIRData(&r, &g, &b, &ir);

  Serial.print("G: ");
  Serial.print(g);
  Serial.print("  Interrupts: ");
  Serial.print(intCount);

  if (intFired) {
    intFired = false;

    // Read status to clear interrupt
    uint8_t status = apds.getMainStatus();

    Serial.print("  *** VARIANCE INTERRUPT! *** Status: 0x");
    Serial.print(status, HEX);
  }

  Serial.println();
  delay(100);
}
