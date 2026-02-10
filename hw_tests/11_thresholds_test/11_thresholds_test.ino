/*!
 * @file 11_thresholds_test.ino
 * @brief Write and read back all threshold registers + functional interrupt
 * test
 */

#include <Adafruit_APDS9999.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16
#define INT_PIN 2

Adafruit_APDS9999 apds;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int passed = 0;
int failed = 0;

volatile bool interruptFired = false;

void interruptHandler() {
  interruptFired = true;
}

void testProxThresholds() {
  Serial.println(F("Testing Proximity Thresholds...\n"));

  // Test values spanning the 11-bit range
  uint16_t test_values[] = {0, 100, 500, 1000, 2047};

  for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
    // Test high threshold
    apds.setProxThresholdHigh(test_values[i]);
    uint16_t readback = apds.getProxThresholdHigh();

    Serial.print(F("Prox High "));
    Serial.print(test_values[i]);
    Serial.print(F(": "));
    if (readback == test_values[i]) {
      Serial.println(F("PASS"));
      passed++;
    } else {
      Serial.print(F("FAIL (got "));
      Serial.print(readback);
      Serial.println(F(")"));
      failed++;
    }

    // Test low threshold
    apds.setProxThresholdLow(test_values[i]);
    readback = apds.getProxThresholdLow();

    Serial.print(F("Prox Low "));
    Serial.print(test_values[i]);
    Serial.print(F(": "));
    if (readback == test_values[i]) {
      Serial.println(F("PASS"));
      passed++;
    } else {
      Serial.print(F("FAIL (got "));
      Serial.print(readback);
      Serial.println(F(")"));
      failed++;
    }
  }
}

void testLightThresholds() {
  Serial.println(F("\nTesting Light Thresholds...\n"));

  // Test values spanning the 20-bit range
  uint32_t test_values[] = {0, 1000, 50000, 500000, 1048575};

  for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
    // Test high threshold
    apds.setLightThresholdHigh(test_values[i]);
    uint32_t readback = apds.getLightThresholdHigh();

    Serial.print(F("Light High "));
    Serial.print(test_values[i]);
    Serial.print(F(": "));
    if (readback == test_values[i]) {
      Serial.println(F("PASS"));
      passed++;
    } else {
      Serial.print(F("FAIL (got "));
      Serial.print(readback);
      Serial.println(F(")"));
      failed++;
    }

    // Test low threshold
    apds.setLightThresholdLow(test_values[i]);
    readback = apds.getLightThresholdLow();

    Serial.print(F("Light Low "));
    Serial.print(test_values[i]);
    Serial.print(F(": "));
    if (readback == test_values[i]) {
      Serial.println(F("PASS"));
      passed++;
    } else {
      Serial.print(F("FAIL (got "));
      Serial.print(readback);
      Serial.println(F(")"));
      failed++;
    }
  }
}

void setNeoPixelsGreen(bool on) {
  if (on) {
    for (int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Full green
    }
  } else {
    pixels.clear();
  }
  pixels.show();
  delay(100); // Allow light to stabilize
}

void testFunctionalThresholds() {
  Serial.println(F("\n========================================"));
  Serial.println(F("Phase 2: Functional Threshold Test"));
  Serial.println(F("========================================\n"));

  // Reset sensor to clean state after register tests
  apds.reset();
  delay(200);
  apds.begin();

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), interruptHandler, FALLING);

  // Enable ALS with known settings (match test 09 settings)
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);
  apds.setLightIntChannel(APDS9999_LIGHT_INT_CH_GREEN);
  apds.setLightVarianceMode(false); // Ensure threshold mode, not variance
  delay(100);                       // Wait for sensor to stabilize

  // --- Calibration ---
  Serial.println(F("Calibrating..."));

  // NeoPixel OFF -> read green baseline
  setNeoPixelsGreen(false);
  delay(200);
  uint32_t r, g, b, ir;
  apds.getRGBIRData(&r, &g, &b, &ir);
  uint32_t baseline = g;
  Serial.print(F("  Baseline (OFF): green="));
  Serial.println(baseline);

  // NeoPixel ON -> read green high
  setNeoPixelsGreen(true);
  delay(200);
  apds.getRGBIRData(&r, &g, &b, &ir);
  uint32_t highVal = g;
  Serial.print(F("  High (ON): green="));
  Serial.println(highVal);

  // Calculate midpoint threshold
  uint32_t midpoint = (baseline + highVal) / 2;
  Serial.print(F("  Midpoint threshold: "));
  Serial.println(midpoint);

  if (highVal <= baseline) {
    Serial.println(
        F("ERROR: No light difference detected! Check NeoPixel wiring."));
    Serial.println(F("Functional High Threshold: SKIP"));
    Serial.println(F("Functional Low Threshold: SKIP"));
    failed += 2;
    setNeoPixelsGreen(false);
    return;
  }

  // --- Test A: High Threshold Crossing ---
  Serial.println(F("\nTest A: High Threshold Crossing"));
  Serial.println(F("  (NeoPixel OFF -> ON should trigger interrupt)"));

  // Set light high threshold to midpoint, low threshold to 0
  apds.setLightThresholdHigh(midpoint);
  apds.setLightThresholdLow(0);
  apds.setLightPersistence(1);
  apds.enableLightInterrupt(true);

  // Start with NeoPixel OFF
  setNeoPixelsGreen(false);
  delay(200);

  // Clear any pending interrupt by reading status
  apds.getMainStatus();
  interruptFired = false;

  // Turn NeoPixel ON (should cross above high threshold)
  setNeoPixelsGreen(true);

  // Wait for interrupt (timeout 2s)
  unsigned long startTime = millis();
  while (!interruptFired && (millis() - startTime < 2000)) {
    delay(10);
  }

  Serial.print(F("  High Threshold Test: "));
  if (interruptFired) {
    Serial.println(F("PASS"));
    passed++;
  } else {
    Serial.println(F("FAIL (no interrupt)"));
    failed++;
  }

  // Clear interrupt by reading status
  apds.getMainStatus();

  // --- Test B: Low Threshold Crossing ---
  Serial.println(F("\nTest B: Low Threshold Crossing"));
  Serial.println(F("  (NeoPixel ON -> OFF should trigger interrupt)"));

  // Set light low threshold to midpoint, high threshold to max (0xFFFFF)
  apds.setLightThresholdLow(midpoint);
  apds.setLightThresholdHigh(0xFFFFF);
  apds.setLightPersistence(1);

  // Clear interrupt
  apds.getMainStatus();
  interruptFired = false;

  // Turn NeoPixel OFF (should cross below low threshold)
  setNeoPixelsGreen(false);

  // Wait for interrupt (timeout 2s)
  startTime = millis();
  while (!interruptFired && (millis() - startTime < 2000)) {
    delay(10);
  }

  Serial.print(F("  Low Threshold Test: "));
  if (interruptFired) {
    Serial.println(F("PASS"));
    passed++;
  } else {
    Serial.println(F("FAIL (no interrupt)"));
    failed++;
  }

  // Cleanup
  apds.enableLightInterrupt(false);
  apds.getMainStatus(); // Clear any pending interrupt
  detachInterrupt(digitalPinToInterrupt(INT_PIN));
  setNeoPixelsGreen(false);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("APDS9999 Thresholds Test"));
  Serial.println(F("------------------------"));

  // Initialize NeoPixels (start OFF)
  pixels.begin();
  pixels.clear();
  pixels.show();

  if (!apds.begin()) {
    Serial.println(F("FAIL: Could not find APDS9999 sensor!"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("APDS9999 found!\n"));

  // Phase 1: Register roundtrip tests
  Serial.println(F("========================================"));
  Serial.println(F("Phase 1: Register Roundtrip Tests"));
  Serial.println(F("========================================\n"));

  testProxThresholds();
  testLightThresholds();

  // Phase 2: Functional threshold tests
  testFunctionalThresholds();

  // Final results
  Serial.println(F("\n========================================"));
  Serial.println(F("Combined Results"));
  Serial.println(F("========================================"));
  Serial.print(F("Passed: "));
  Serial.print(passed);
  Serial.print(F(" / Failed: "));
  Serial.println(failed);
  Serial.println(failed == 0 ? F("*** TEST PASSED ***")
                             : F("*** TEST FAILED ***"));
}

void loop() {
  delay(1000);
}
