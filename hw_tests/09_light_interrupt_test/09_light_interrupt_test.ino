/*!
 * @file 09_light_interrupt_test.ino
 * @brief Self-calibrating light sensor interrupt test using NeoPixel ring
 * @note Connect INT pin to D2, NeoPixel ring to D6
 */

#include <Adafruit_APDS9999.h>
#include <Adafruit_NeoPixel.h>

#define INT_PIN 2
#define NEOPIXEL_PIN 6
#define NUM_PIXELS 16
#define TIMEOUT_MS 3000
#define CALIBRATION_SAMPLES 5
#define SAMPLE_DELAY_MS 50
#define SETTLE_DELAY_MS 300

Adafruit_APDS9999 apds;
Adafruit_NeoPixel ring(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

volatile bool intFired = false;

void isr() { intFired = true; }

void setRingColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
  ring.show();
}

bool waitForInterrupt(uint32_t timeoutMs) {
  uint32_t start = millis();
  while (!intFired) {
    if (millis() - start > timeoutMs) {
      return false;
    }
    delay(10);
  }
  return true;
}

uint32_t readGreenAverage(int samples) {
  uint32_t sum = 0;
  uint32_t r, g, b, ir;

  for (int i = 0; i < samples; i++) {
    apds.getRGBIRData(&r, &g, &b, &ir);
    sum += g;
    delay(SAMPLE_DELAY_MS);
  }
  return sum / samples;
}

void clearInterruptState() {
  apds.getMainStatus();
  intFired = false;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("APDS9999 Self-Calibrating Light Interrupt Test"));
  Serial.println(F("==============================================="));

  // Initialize NeoPixel ring - OFF initially
  ring.begin();
  ring.setBrightness(255);
  setRingColor(0, 0, 0);

  if (!apds.begin()) {
    Serial.println(F("FAIL: Could not find APDS9999 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("APDS9999 found!"));

  // Enable light sensor
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);

  // Wait for sensor to stabilize
  delay(200);

  // ============ PHASE 1: CALIBRATION ============
  Serial.println(F("\n--- Phase 1: Calibration ---"));

  // Step 1: NeoPixels OFF - get baseline LOW value
  setRingColor(0, 0, 0);
  delay(SETTLE_DELAY_MS);
  uint32_t baseline = readGreenAverage(CALIBRATION_SAMPLES);
  Serial.print(F("Baseline (NeoPixels OFF): G="));
  Serial.println(baseline);

  // Step 2: NeoPixels ON bright white - get HIGH value
  setRingColor(255, 255, 255);
  delay(SETTLE_DELAY_MS);
  uint32_t highValue = readGreenAverage(CALIBRATION_SAMPLES);
  Serial.print(F("High value (NeoPixels ON): G="));
  Serial.println(highValue);

  // Step 3: Calculate thresholds (25% and 75% of range)
  uint32_t range = highValue - baseline;
  uint32_t lowThresh = baseline + (range / 4);
  uint32_t highThresh = baseline + ((range * 3) / 4);

  Serial.print(F("Calculated low threshold:  "));
  Serial.println(lowThresh);
  Serial.print(F("Calculated high threshold: "));
  Serial.println(highThresh);

  // Sanity check
  if (highValue <= baseline || range < 100) {
    Serial.println(F("FAIL: Insufficient light range for calibration!"));
    setRingColor(0, 0, 0);
    while (1) {
      delay(10);
    }
  }

  // ============ PHASE 2: INTERRUPT TEST ============
  Serial.println(F("\n--- Phase 2: Interrupt Test ---"));

  // Configure interrupt thresholds
  apds.setLightThresholdLow(lowThresh);
  apds.setLightThresholdHigh(highThresh);

  // Use green channel, persistence=4
  apds.setLightIntChannel(APDS9999_LIGHT_INT_CH_GREEN);
  apds.setLightPersistence(4);
  apds.enableLightInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, FALLING);

  // ============ TEST 1: NeoPixels OFF - Light Decrease ============
  Serial.println(F("\n--- Test 1: NeoPixels OFF (expect LOW interrupt) ---"));

  // NeoPixels are currently ON from calibration
  // Turn OFF, wait for sensor to settle, then clear and wait for interrupt
  setRingColor(0, 0, 0);
  Serial.println(F("NeoPixels OFF - waiting for sensor settle..."));
  delay(SETTLE_DELAY_MS);
  clearInterruptState();

  bool test1Pass = false;
  uint32_t r, g, b, ir;

  if (waitForInterrupt(TIMEOUT_MS)) {
    uint8_t status = apds.getMainStatus();
    delay(50);
    apds.getRGBIRData(&r, &g, &b, &ir);

    Serial.print(F("Interrupt fired! G="));
    Serial.print(g);
    Serial.print(F(" Status=0x"));
    Serial.println(status, HEX);

    if (g < lowThresh) {
      Serial.println(F("PASS: Green below low threshold"));
      test1Pass = true;
    } else {
      Serial.println(F("FAIL: Green NOT below low threshold"));
    }
  } else {
    Serial.println(F("FAIL: Timeout waiting for LOW interrupt"));
  }

  // ============ TEST 2: NeoPixels ON - Light Increase ============
  Serial.println(F("\n--- Test 2: NeoPixels ON (expect HIGH interrupt) ---"));

  // Turn ON, wait for sensor to settle, then clear and wait for interrupt
  setRingColor(255, 255, 255);
  Serial.println(F("NeoPixels ON - waiting for sensor settle..."));
  delay(SETTLE_DELAY_MS);
  clearInterruptState();

  bool test2Pass = false;

  if (waitForInterrupt(TIMEOUT_MS)) {
    uint8_t status = apds.getMainStatus();
    delay(50);
    apds.getRGBIRData(&r, &g, &b, &ir);

    Serial.print(F("Interrupt fired! G="));
    Serial.print(g);
    Serial.print(F(" Status=0x"));
    Serial.println(status, HEX);

    if (g > highThresh) {
      Serial.println(F("PASS: Green above high threshold"));
      test2Pass = true;
    } else {
      Serial.println(F("FAIL: Green NOT above high threshold"));
    }
  } else {
    Serial.println(F("FAIL: Timeout waiting for HIGH interrupt"));
  }

  // ============ SUMMARY ============
  Serial.println(F("\n==============================================="));
  Serial.println(F("TEST SUMMARY"));
  Serial.println(F("==============================================="));
  Serial.print(F("Test 1 (Low Interrupt):  "));
  Serial.println(test1Pass ? F("PASS") : F("FAIL"));
  Serial.print(F("Test 2 (High Interrupt): "));
  Serial.println(test2Pass ? F("PASS") : F("FAIL"));
  Serial.println(F("-----------------------------------------------"));

  if (test1Pass && test2Pass) {
    Serial.println(F("OVERALL: PASS"));
  } else {
    Serial.println(F("OVERALL: FAIL"));
  }

  // Cleanup - NeoPixels OFF
  setRingColor(0, 0, 0);
}

void loop() {
  // Test complete - nothing to do in loop
  delay(1000);
}
