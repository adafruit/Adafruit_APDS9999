/*!
 * @file 17_prox_logic_mode_test.ino
 * @brief Test proximity logic mode (INT pin behavior) with servo automation
 * @note Hardware: Servo on A0, INT pin on D2
 *
 * Logic Mode OFF: INT goes LOW on threshold crossing, LATCHES until cleared
 * Logic Mode ON: INT follows proximity state directly (no latch)
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

#define INT_PIN 2
#define SERVO_PIN A0
#define SERVO_AWAY 0
#define SERVO_CLOSE 105

Adafruit_APDS9999 apds;
Servo servo;

bool phase1_pass = false;
bool phase2_pass = false;
bool phase3_pass = false;

void moveServo(int angle, uint16_t settleMs = 500) {
  servo.write(angle);
  delay(settleMs);
}

uint16_t readProxAvg(uint8_t samples = 5) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t p;
    apds.readProximity(&p);
    sum += p;
    delay(50);
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Proximity Logic Mode Test (Servo Automated)");
  Serial.println("=====================================================");
  Serial.println("Hardware: Servo on A0, INT on D2\n");

  // Initialize servo
  servo.attach(SERVO_PIN);
  moveServo(SERVO_AWAY, 1000);

  // Initialize sensor
  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    servo.write(SERVO_AWAY);
    servo.detach();
    while (1) {
      delay(10);
    }
  }
  Serial.println("APDS9999 found!");

  // Setup INT pin
  pinMode(INT_PIN, INPUT_PULLUP);

  // Enable proximity sensor
  apds.enableProximitySensor(true);
  apds.setProxPersistence(1); // Fast response
  delay(100);

  // =========================================================================
  // PHASE 1: CALIBRATION
  // =========================================================================
  Serial.println("\n=== PHASE 1: CALIBRATION ===");

  // Read baseline (servo away)
  moveServo(SERVO_AWAY, 800);
  uint16_t baseline = readProxAvg(8);
  Serial.print("Baseline (servo away): ");
  Serial.println(baseline);

  // Read high value (servo close)
  moveServo(SERVO_CLOSE, 800);
  uint16_t highProx = readProxAvg(8);
  Serial.print("High prox (servo close): ");
  Serial.println(highProx);

  // Calculate threshold at 50%
  uint16_t threshold = (baseline + highProx) / 2;
  Serial.print("Threshold (50%): ");
  Serial.println(threshold);

  // Verify we have useful range
  if (highProx > baseline + 50) {
    Serial.println("PHASE 1: PASS - Good calibration range");
    phase1_pass = true;
  } else {
    Serial.println("PHASE 1: FAIL - Insufficient prox range");
    phase1_pass = false;
  }

  // Return servo to away position
  moveServo(SERVO_AWAY, 500);

  // =========================================================================
  // PHASE 2: LOGIC MODE OFF (LATCHING)
  // =========================================================================
  Serial.println("\n=== PHASE 2: LOGIC MODE OFF (LATCHING) ===");

  apds.setProxLogicMode(false);
  bool modeOff = apds.getProxLogicMode();
  Serial.print("Logic mode set to OFF: ");
  Serial.println(modeOff ? "FAIL" : "OK");

  // Set thresholds
  apds.setProxThresholdLow(0);
  apds.setProxThresholdHigh(threshold);
  apds.enableProxInterrupt(true);

  // Clear any pending interrupt
  apds.getMainStatus();
  delay(100);

  // Servo at away position, INT should be HIGH
  moveServo(SERVO_AWAY, 500);
  apds.getMainStatus(); // Clear again
  delay(100);

  bool intBefore = digitalRead(INT_PIN);
  Serial.print("INT before crossing (should be HIGH): ");
  Serial.println(intBefore ? "HIGH - OK" : "LOW - unexpected");

  // Move servo close to cross threshold
  Serial.println("Moving servo to close position...");
  moveServo(SERVO_CLOSE, 600);

  bool intAfterCross = digitalRead(INT_PIN);
  Serial.print("INT after crossing (should be LOW): ");
  Serial.println(intAfterCross ? "HIGH - FAIL" : "LOW - OK");

  // Check that INT LATCHES (stays LOW without clearing)
  delay(300);
  bool intStillLow = digitalRead(INT_PIN);
  Serial.print("INT still latched LOW: ");
  Serial.println(intStillLow ? "HIGH - FAIL (not latched)"
                             : "LOW - OK (latched)");

  // Read status to clear the interrupt
  Serial.println("Reading status to clear interrupt...");
  uint8_t status = apds.getMainStatus();
  delay(100);

  bool intAfterClear = digitalRead(INT_PIN);
  Serial.print("INT after status clear (should be HIGH): ");
  Serial.println(intAfterClear ? "HIGH - OK" : "LOW - FAIL");

  // Evaluate Phase 2
  if (intBefore && !intAfterCross && !intStillLow && intAfterClear) {
    Serial.println("PHASE 2: PASS - Latching behavior correct");
    phase2_pass = true;
  } else {
    Serial.println("PHASE 2: FAIL - Latching behavior incorrect");
    phase2_pass = false;
  }

  // Return servo
  moveServo(SERVO_AWAY, 500);
  apds.getMainStatus(); // Clear

  // =========================================================================
  // PHASE 3: LOGIC MODE ON (FOLLOWING)
  // =========================================================================
  Serial.println("\n=== PHASE 3: LOGIC MODE ON (FOLLOWING) ===");

  apds.setProxLogicMode(true);
  bool modeOn = apds.getProxLogicMode();
  Serial.print("Logic mode set to ON: ");
  Serial.println(modeOn ? "OK" : "FAIL");

  // Clear status
  apds.getMainStatus();
  delay(100);

  // Servo away - INT should be HIGH (below threshold)
  moveServo(SERVO_AWAY, 600);
  bool intAway1 = digitalRead(INT_PIN);
  Serial.print("Servo AWAY -> INT (should be HIGH): ");
  Serial.println(intAway1 ? "HIGH - OK" : "LOW - FAIL");

  // Servo close - INT should go LOW (above threshold)
  moveServo(SERVO_CLOSE, 600);
  bool intClose1 = digitalRead(INT_PIN);
  Serial.print("Servo CLOSE -> INT (should be LOW): ");
  Serial.println(intClose1 ? "HIGH - FAIL" : "LOW - OK");

  // Servo back away - INT should go HIGH again (no latch!)
  moveServo(SERVO_AWAY, 600);
  bool intAway2 = digitalRead(INT_PIN);
  Serial.print("Servo AWAY again -> INT (should be HIGH, no latch): ");
  Serial.println(intAway2 ? "HIGH - OK" : "LOW - FAIL (still latched!)");

  // One more cycle to confirm tracking
  moveServo(SERVO_CLOSE, 500);
  bool intClose2 = digitalRead(INT_PIN);
  moveServo(SERVO_AWAY, 500);
  bool intAway3 = digitalRead(INT_PIN);

  Serial.print("Second cycle - CLOSE: ");
  Serial.print(intClose2 ? "HIGH" : "LOW");
  Serial.print(", AWAY: ");
  Serial.println(intAway3 ? "HIGH" : "LOW");

  // Evaluate Phase 3
  if (intAway1 && !intClose1 && intAway2 && !intClose2 && intAway3) {
    Serial.println("PHASE 3: PASS - Following behavior correct");
    phase3_pass = true;
  } else {
    Serial.println("PHASE 3: FAIL - Following behavior incorrect");
    phase3_pass = false;
  }

  // =========================================================================
  // SUMMARY
  // =========================================================================
  Serial.println("\n========== TEST SUMMARY ==========");
  Serial.print("Phase 1 (Calibration):      ");
  Serial.println(phase1_pass ? "PASS" : "FAIL");
  Serial.print("Phase 2 (Logic Mode OFF):   ");
  Serial.println(phase2_pass ? "PASS" : "FAIL");
  Serial.print("Phase 3 (Logic Mode ON):    ");
  Serial.println(phase3_pass ? "PASS" : "FAIL");
  Serial.println("==================================");

  if (phase1_pass && phase2_pass && phase3_pass) {
    Serial.println("OVERALL: PASS");
  } else {
    Serial.println("OVERALL: FAIL");
  }

  // Cleanup
  Serial.println("\nCleaning up...");
  moveServo(SERVO_AWAY, 500);
  servo.detach();
  Serial.println("Servo detached. Test complete.");
}

void loop() {
  // Test runs once in setup
  delay(1000);
}
