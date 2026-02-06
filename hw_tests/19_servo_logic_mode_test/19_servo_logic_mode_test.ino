/*!
 * @file 19_servo_logic_mode_test.ino
 * @brief Automated test demonstrating logic mode OFF vs ON behavior
 * @note Connect INT pin to D2, Servo to A0
 *       Servo 105° = paper in front of sensor (object detected)
 *       Servo 0° = paper away from sensor (no object)
 *
 * Logic Mode OFF (normal interrupt):
 *   - INT pin latches LOW when threshold crossed
 *   - Stays LOW until status register is read (clears it)
 *
 * Logic Mode ON (real-time):
 *   - INT pin follows real-time proximity state
 *   - HIGH when object far, LOW when object near
 *   - Does NOT latch - goes back HIGH when object removed
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

#define INT_PIN 2
#define SERVO_PIN A0

Adafruit_APDS9999 apds;
Servo testServo;

// Test results
bool passLogicOffLatch = false;
bool passLogicOffClear = false;
bool passLogicOnFollow = false;
bool passLogicOnNoLatch = false;

/**
 * @brief Move servo to specified angle and wait for settling
 * @param angle 105 = paper in front of sensor, 0 = paper away
 */
void moveServo(int angle) {
  testServo.write(angle);
  delay(700);
}

void printIntState(bool intPin, bool expected) {
  Serial.print(F("  INT: "));
  Serial.print(intPin ? F("HIGH") : F("LOW"));
  Serial.print(intPin == expected ? F(" OK") : F(" BAD"));
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println();
  Serial.println(F("APDS9999 Automated Logic Mode Test"));
  Serial.println(F("===================================="));
  Serial.println(F("INT=D2, Servo=A0, 105deg=front, 0deg=away"));

  // Initialize servo
  testServo.attach(SERVO_PIN);
  moveServo(0);
  Serial.println(F("Servo at 0deg"));

  // Initialize sensor
  if (!apds.begin()) {
    Serial.println(F("FAIL: No sensor!"));
    while (1) {
      delay(10);
    }
  }
  Serial.println(F("APDS9999 found!"));

  // Configure sensor
  apds.enableProximitySensor(true);
  apds.setProxThresholdLow(10);
  apds.setProxThresholdHigh(20);
  apds.setProxPersistence(1);
  apds.enableProxInterrupt(true);

  pinMode(INT_PIN, INPUT_PULLUP);
  apds.getMainStatus();

  Serial.println(F("Thresh: L=10, H=20"));

  // ========================================
  // PART A: Logic Mode OFF (Latching)
  // ========================================
  Serial.println();
  Serial.println(F("=== PART A: Logic OFF (Latching) ==="));

  apds.setProxLogicMode(false);
  Serial.print(F("Mode: "));
  Serial.println(apds.getProxLogicMode() ? F("ON") : F("OFF"));

  apds.getMainStatus();
  delay(200);

  // A1: Away, expect HIGH
  Serial.println(F("\nA1: Servo 0deg (away)"));
  moveServo(0);
  delay(500);
  bool intA1 = digitalRead(INT_PIN);
  uint16_t proxA1;
  apds.readProximity(&proxA1);
  Serial.print(F("  Prox: "));
  Serial.println(proxA1);
  printIntState(intA1, HIGH);

  apds.getMainStatus();
  delay(100);

  // A2: Front, expect LOW
  Serial.println(F("\nA2: Servo 105deg (front)"));
  moveServo(105);
  delay(500);
  bool intA2 = digitalRead(INT_PIN);
  uint16_t proxA2;
  apds.readProximity(&proxA2);
  Serial.print(F("  Prox: "));
  Serial.println(proxA2);
  printIntState(intA2, LOW);

  // A3: Back away, expect STILL LOW (latched!)
  Serial.println(F("\nA3: Servo 0deg (away) - KEY TEST"));
  Serial.println(F("  Should STAY LOW (latched)!"));
  moveServo(0);
  delay(500);
  bool intA3 = digitalRead(INT_PIN);
  uint16_t proxA3;
  apds.readProximity(&proxA3);
  Serial.print(F("  Prox: "));
  Serial.println(proxA3);
  printIntState(intA3, LOW);
  passLogicOffLatch = (intA3 == LOW);
  Serial.print(F("  Latch test: "));
  Serial.println(passLogicOffLatch ? F("PASS") : F("FAIL"));

  // A4: Clear with status read, expect HIGH
  Serial.println(F("\nA4: Read status to clear"));
  uint8_t statusA4 = apds.getMainStatus();
  delay(100);
  bool intA4 = digitalRead(INT_PIN);
  Serial.print(F("  Status: 0x"));
  Serial.println(statusA4, HEX);
  printIntState(intA4, HIGH);
  passLogicOffClear = (intA4 == HIGH);
  Serial.print(F("  Clear test: "));
  Serial.println(passLogicOffClear ? F("PASS") : F("FAIL"));

  // ========================================
  // PART B: Logic Mode ON (Real-time)
  // ========================================
  Serial.println();
  Serial.println(F("=== PART B: Logic ON (Real-time) ==="));

  apds.setProxLogicMode(true);
  Serial.print(F("Mode: "));
  Serial.println(apds.getProxLogicMode() ? F("ON") : F("OFF"));

  apds.getMainStatus();
  delay(200);

  // B1: Away, expect HIGH
  Serial.println(F("\nB1: Servo 0deg (away)"));
  moveServo(0);
  delay(500);
  bool intB1 = digitalRead(INT_PIN);
  uint16_t proxB1;
  apds.readProximity(&proxB1);
  Serial.print(F("  Prox: "));
  Serial.println(proxB1);
  printIntState(intB1, HIGH);

  // B2: Front, expect LOW
  Serial.println(F("\nB2: Servo 105deg (front)"));
  moveServo(105);
  delay(500);
  bool intB2 = digitalRead(INT_PIN);
  uint16_t proxB2;
  apds.readProximity(&proxB2);
  Serial.print(F("  Prox: "));
  Serial.println(proxB2);
  printIntState(intB2, LOW);
  passLogicOnFollow = (intB2 == LOW);
  Serial.print(F("  Follow test: "));
  Serial.println(passLogicOnFollow ? F("PASS") : F("FAIL"));

  // B3: Back away, expect HIGH (NOT latched!)
  Serial.println(F("\nB3: Servo 0deg (away) - KEY TEST"));
  Serial.println(F("  Should go HIGH (not latched)!"));
  moveServo(0);
  delay(500);
  bool intB3 = digitalRead(INT_PIN);
  uint16_t proxB3;
  apds.readProximity(&proxB3);
  Serial.print(F("  Prox: "));
  Serial.println(proxB3);
  printIntState(intB3, HIGH);
  passLogicOnNoLatch = (intB3 == HIGH);
  Serial.print(F("  No-latch test: "));
  Serial.println(passLogicOnNoLatch ? F("PASS") : F("FAIL"));

  // ========================================
  // SUMMARY
  // ========================================
  Serial.println();
  Serial.println(F("========== SUMMARY =========="));
  Serial.print(F("OFF-Latch:    "));
  Serial.println(passLogicOffLatch ? F("PASS") : F("FAIL"));
  Serial.print(F("OFF-Clear:    "));
  Serial.println(passLogicOffClear ? F("PASS") : F("FAIL"));
  Serial.print(F("ON-Follow:    "));
  Serial.println(passLogicOnFollow ? F("PASS") : F("FAIL"));
  Serial.print(F("ON-NoLatch:   "));
  Serial.println(passLogicOnNoLatch ? F("PASS") : F("FAIL"));

  bool allPass = passLogicOffLatch && passLogicOffClear && passLogicOnFollow &&
                 passLogicOnNoLatch;

  Serial.println();
  Serial.print(F("OVERALL: "));
  Serial.println(allPass ? F("PASS") : F("FAIL"));
  Serial.println(F("============================="));

  moveServo(0);
}

void loop() {
  delay(1000);
}
