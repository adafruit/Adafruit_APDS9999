/*!
 * @file 14_status_test.ino
 * @brief Hardware test for status register flags using servo and NeoPixel
 *
 * Hardware Setup:
 * - Servo on A0 (0°=away, 105°=close to sensor)
 * - NeoPixel on D6 (16 pixels)
 * - INT pin D2 (reference only)
 */

#include <Adafruit_APDS9999.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define SERVO_PIN A0
#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16

#define SERVO_AWAY 0
#define SERVO_CLOSE 105

Adafruit_APDS9999 apds;
Servo servo;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int passed = 0;
int failed = 0;

void decodeStatus(uint8_t status) {
  Serial.print("Status: 0x");
  Serial.print(status, HEX);
  Serial.print(" -> ");

  if (status & APDS9999_STATUS_POWER_ON) {
    Serial.print("[POWER_ON] ");
  }
  if (status & APDS9999_STATUS_LIGHT_INT) {
    Serial.print("[LIGHT_INT] ");
  }
  if (status & APDS9999_STATUS_LIGHT_DATA) {
    Serial.print("[LIGHT_DATA] ");
  }
  if (status & APDS9999_STATUS_PROX_LOGIC) {
    Serial.print("[PROX_LOGIC] ");
  }
  if (status & APDS9999_STATUS_PROX_INT) {
    Serial.print("[PROX_INT] ");
  }
  if (status & APDS9999_STATUS_PROX_DATA) {
    Serial.print("[PROX_DATA] ");
  }

  if (status == 0) {
    Serial.print("(no flags set)");
  }
  Serial.println();
}

void setNeoPixels(bool on, uint8_t r, uint8_t g, uint8_t b) {
  if (on) {
    for (int i = 0; i < NEOPIXEL_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
  } else {
    pixels.clear();
  }
  pixels.show();
}

void moveServo(int angle) {
  servo.write(angle);
  delay(500); // Wait for servo to reach position
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("=========================================="));
  Serial.println(F("APDS9999 Status Register Hardware Test"));
  Serial.println(F("=========================================="));

  // Initialize hardware
  pixels.begin();
  pixels.setBrightness(255);
  setNeoPixels(false, 0, 0, 0);

  servo.attach(SERVO_PIN);
  moveServo(SERVO_AWAY);

  if (!apds.begin()) {
    Serial.println(F("FAIL: Could not find APDS9999 sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("APDS9999 found!\n"));

  // ===== TEST 1: POWER_ON flag and clear-on-read (INFO ONLY) =====
  Serial.println(F("------------------------------------------"));
  Serial.println(
      F("TEST 1: POWER_ON flag (INFO - requires fresh power cycle)"));
  Serial.println(F("------------------------------------------"));

  uint8_t status1 = apds.getMainStatus();
  Serial.print(F("First read: "));
  decodeStatus(status1);

  uint8_t status2 = apds.getMainStatus();
  Serial.print(F("Second read: "));
  decodeStatus(status2);

  bool hasPowerOn = (status1 & APDS9999_STATUS_POWER_ON);
  bool clearedOnRead = !(status2 & APDS9999_STATUS_POWER_ON);

  if (hasPowerOn && clearedOnRead) {
    Serial.println(F("TEST 1: INFO - POWER_ON set then cleared (fresh boot)"));
  } else {
    Serial.println(
        F("TEST 1: INFO - POWER_ON already cleared (not fresh boot)"));
  }
  // Not counted as pass/fail - depends on power cycle state

  // ===== TEST 2: PROX_DATA ready flag =====
  Serial.println(F("\n------------------------------------------"));
  Serial.println(F("TEST 2: PROX_DATA ready flag"));
  Serial.println(F("------------------------------------------"));

  apds.enableProximitySensor(true);
  delay(200);

  uint8_t proxStatus = apds.getMainStatus();
  Serial.print(F("After enabling prox: "));
  decodeStatus(proxStatus);

  if (proxStatus & APDS9999_STATUS_PROX_DATA) {
    Serial.println(F("TEST 2: PASS - PROX_DATA flag set"));
    passed++;
  } else {
    Serial.println(F("TEST 2: FAIL - PROX_DATA flag not set"));
    failed++;
  }

  // ===== TEST 3: LIGHT_DATA ready flag =====
  Serial.println(F("\n------------------------------------------"));
  Serial.println(F("TEST 3: LIGHT_DATA ready flag"));
  Serial.println(F("------------------------------------------"));

  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  delay(200);

  uint8_t lightStatus = apds.getMainStatus();
  Serial.print(F("After enabling light: "));
  decodeStatus(lightStatus);

  if (lightStatus & APDS9999_STATUS_LIGHT_DATA) {
    Serial.println(F("TEST 3: PASS - LIGHT_DATA flag set"));
    passed++;
  } else {
    Serial.println(F("TEST 3: FAIL - LIGHT_DATA flag not set"));
    failed++;
  }

  // ===== TEST 4: PROX_INT flag (servo triggered) =====
  Serial.println(F("\n------------------------------------------"));
  Serial.println(F("TEST 4: PROX_INT flag (servo triggered)"));
  Serial.println(F("------------------------------------------"));

  // Configure prox interrupt with low threshold
  apds.setProxThresholdLow(20);
  apds.setProxThresholdHigh(255);
  apds.setProxPersistence(1); // Trigger after 1 sample
  apds.enableProxInterrupt(true);

  // Start with servo away, clear any pending status
  Serial.println(F("Servo at 0 deg (away)..."));
  moveServo(SERVO_AWAY);
  delay(300);
  apds.getMainStatus(); // Clear status

  // Move servo close to trigger threshold crossing
  Serial.println(F("Moving servo to 105 deg (close)..."));
  moveServo(SERVO_CLOSE);
  delay(500);

  uint8_t proxIntStatus = apds.getMainStatus();
  Serial.print(F("Status after threshold crossing: "));
  decodeStatus(proxIntStatus);

  if (proxIntStatus & APDS9999_STATUS_PROX_INT) {
    Serial.println(F("TEST 4: PASS - PROX_INT flag set"));
    passed++;
  } else {
    Serial.println(F("TEST 4: FAIL - PROX_INT flag not set"));
    failed++;
  }

  // ===== TEST 5: PROX_LOGIC flag behavior =====
  Serial.println(F("\n------------------------------------------"));
  Serial.println(F("TEST 5: PROX_LOGIC flag behavior"));
  Serial.println(F("------------------------------------------"));

  // PROX_LOGIC_STAT: 1 when prox > high threshold, 0 when prox < low threshold
  // Servo readings: ~0-5 away, ~60 close
  // Set thresholds so: close (60) > high (40), away (0) < low (30)
  apds.setProxThresholdLow(30);
  apds.setProxThresholdHigh(40);
  Serial.println(F("Thresholds: low=30, high=40 (away~0, close~60)"));

  // Move servo close first - prox should exceed high threshold
  Serial.println(F("Moving servo to 105 deg (close)..."));
  moveServo(SERVO_CLOSE);
  delay(300);

  // Clear status and read with servo close
  apds.getMainStatus();
  delay(200);
  uint8_t closeStatus = apds.getMainStatus();
  Serial.print(F("Servo at 105 deg (close): "));
  decodeStatus(closeStatus);
  bool proxLogicClose = (closeStatus & APDS9999_STATUS_PROX_LOGIC);

  // Move servo away - prox should go below low threshold
  Serial.println(F("Moving servo to 0 deg (away)..."));
  moveServo(SERVO_AWAY);
  delay(500);

  apds.getMainStatus(); // Clear
  delay(200);
  uint8_t awayStatus = apds.getMainStatus();
  Serial.print(F("Servo at 0 deg (away): "));
  decodeStatus(awayStatus);
  bool proxLogicAway = (awayStatus & APDS9999_STATUS_PROX_LOGIC);

  Serial.print(F("PROX_LOGIC close="));
  Serial.print(proxLogicClose ? "1" : "0");
  Serial.print(F(", away="));
  Serial.println(proxLogicAway ? "1" : "0");

  if (proxLogicClose && !proxLogicAway) {
    Serial.println(F("TEST 5: PASS - PROX_LOGIC changes with distance"));
    passed++;
  } else {
    Serial.println(F("TEST 5: FAIL - PROX_LOGIC not changing as expected"));
    failed++;
  }

  // ===== TEST 6: LIGHT_INT flag (NeoPixel triggered) =====
  Serial.println(F("\n------------------------------------------"));
  Serial.println(F("TEST 6: LIGHT_INT flag (NeoPixel triggered)"));
  Serial.println(F("------------------------------------------"));

  // Calibrate: measure light OFF and ON
  setNeoPixels(false, 0, 0, 0);
  delay(300);
  apds.getMainStatus();
  delay(200);
  uint32_t r, g, b, ir;
  apds.getRGBIRData(&r, &g, &b, &ir);
  uint32_t lightOff = g; // Use green channel for light level
  Serial.print(F("Light OFF - Green: "));
  Serial.println(lightOff);

  setNeoPixels(true, 255, 255, 255);
  delay(300);
  apds.getMainStatus();
  delay(200);
  apds.getRGBIRData(&r, &g, &b, &ir);
  uint32_t lightOn = g;
  Serial.print(F("Light ON - Green: "));
  Serial.println(lightOn);

  // Set threshold between OFF and ON levels
  uint32_t threshold = (lightOn / 2); // Use half of ON level as threshold
  Serial.print(F("Setting threshold: "));
  Serial.println(threshold);

  apds.setLightThresholdLow(0);
  apds.setLightThresholdHigh(threshold);
  apds.setLightPersistence(1);
  apds.enableLightInterrupt(true);

  // Start with NeoPixels OFF, clear status
  setNeoPixels(false, 0, 0, 0);
  delay(300);
  apds.getMainStatus(); // Clear

  // Turn NeoPixels ON to trigger threshold
  Serial.println(F("Turning NeoPixels ON..."));
  setNeoPixels(true, 255, 255, 255);
  delay(500);

  uint8_t lightIntStatus = apds.getMainStatus();
  Serial.print(F("Status after light change: "));
  decodeStatus(lightIntStatus);

  if (lightIntStatus & APDS9999_STATUS_LIGHT_INT) {
    Serial.println(F("TEST 6: PASS - LIGHT_INT flag set"));
    passed++;
  } else {
    Serial.println(F("TEST 6: FAIL - LIGHT_INT flag not set"));
    failed++;
  }

  // ===== CLEANUP =====
  Serial.println(F("\n------------------------------------------"));
  Serial.println(F("CLEANUP"));
  Serial.println(F("------------------------------------------"));

  setNeoPixels(false, 0, 0, 0);
  moveServo(SERVO_AWAY);
  servo.detach();

  // ===== RESULTS =====
  Serial.println(F("\n=========================================="));
  Serial.println(F("TEST RESULTS"));
  Serial.println(F("=========================================="));
  Serial.print(F("Passed: "));
  Serial.println(passed);
  Serial.print(F("Failed: "));
  Serial.println(failed);
  Serial.print(F("Total:  "));
  Serial.println(passed + failed);

  if (failed == 0) {
    Serial.println(F("\n*** ALL TESTS PASSED ***"));
  } else {
    Serial.println(F("\n*** SOME TESTS FAILED ***"));
  }
}

void loop() {
  // Nothing to do
  delay(1000);
}
