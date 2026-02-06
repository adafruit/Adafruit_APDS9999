/*!
 * @file 13_cancellation_test.ino
 * @brief Test prox cancellation (digital 0-2047, analog 0-7)
 *
 * Uses servo to position paper for consistent functional testing.
 * Verifies that cancellation settings actually reduce proximity readings.
 *
 * Hardware: INT=D2, Servo=A0 (105°=paper in front, 0°=away)
 */

#include <Adafruit_APDS9999.h>
#include <Servo.h>

Adafruit_APDS9999 apds;
Servo servo;

const int SERVO_PIN = A0;
const int SERVO_PAPER_IN = 105;
const int SERVO_PAPER_OUT = 0;

int passed = 0;
int failed = 0;

void moveServo(int angle) {
  servo.write(angle);
  delay(500); // Wait for servo to settle
}

uint16_t getAvgProx(int samples = 5) {
  uint32_t sum = 0;
  uint16_t prox;
  for (int i = 0; i < samples; i++) {
    delay(50);
    apds.readProximity(&prox);
    sum += prox;
  }
  return sum / samples;
}

void testDigitalCancellation() {
  Serial.println("Testing Digital Cancellation (11-bit, 0-2047)...\n");

  // Test values spanning the 11-bit range (0-2047)
  uint16_t test_values[] = {0, 100, 500, 1000, 1500, 2047};

  for (size_t i = 0; i < sizeof(test_values) / sizeof(test_values[0]); i++) {
    apds.setProxCancellation(test_values[i]);
    uint16_t readback = apds.getProxCancellation();

    Serial.print("Digital Cancel ");
    Serial.print(test_values[i]);
    Serial.print(": ");
    if (readback == test_values[i]) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }
}

void testAnalogCancellation() {
  Serial.println("\nTesting Analog Cancellation (0-7)...\n");

  for (uint8_t a = 0; a <= 7; a++) {
    apds.setProxAnalogCancellation(a);
    uint8_t readback = apds.getProxAnalogCancellation();

    Serial.print("Analog Cancel ");
    Serial.print(a);
    Serial.print(": ");
    if (readback == a) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }
}

void testDigitalCancellationEffect() {
  Serial.println("\n=== FUNCTIONAL TEST: Digital Cancellation Effect ===\n");

  // Position paper in front of sensor for consistent target
  Serial.println("Moving paper in front of sensor...");
  moveServo(SERVO_PAPER_IN);
  delay(300);

  // Reset cancellation
  apds.setProxCancellation(0);
  apds.setProxAnalogCancellation(0);
  delay(100);

  // Get baseline reading
  uint16_t baseline = getAvgProx(5);
  Serial.print("Baseline (cancel=0): ");
  Serial.println(baseline);

  // Test with small cancellation value first (10)
  uint16_t low_cancel = 10;
  apds.setProxCancellation(low_cancel);
  delay(100);
  uint16_t with_low_cancel = getAvgProx(5);
  Serial.print("With cancel=");
  Serial.print(low_cancel);
  Serial.print(": ");
  Serial.println(with_low_cancel);

  // Expected: reading should be reduced
  Serial.print("Digital cancellation reduces reading: ");
  if (with_low_cancel < baseline) {
    int16_t reduction = baseline - with_low_cancel;
    Serial.print("PASS (reduced by ");
    Serial.print(reduction);
    Serial.println(")");
    passed++;
  } else {
    Serial.print("FAIL (");
    Serial.print(with_low_cancel);
    Serial.print(" not < ");
    Serial.print(baseline);
    Serial.println(")");
    failed++;
  }

  // Test with higher cancellation - should reduce more
  uint16_t high_cancel = 30;
  apds.setProxCancellation(high_cancel);
  delay(100);
  uint16_t with_high_cancel = getAvgProx(5);
  Serial.print("With cancel=");
  Serial.print(high_cancel);
  Serial.print(": ");
  Serial.println(with_high_cancel);

  Serial.print("Higher cancellation reduces more: ");
  if (with_high_cancel < with_low_cancel) {
    Serial.print("PASS (");
    Serial.print(with_high_cancel);
    Serial.print(" < ");
    Serial.print(with_low_cancel);
    Serial.println(")");
    passed++;
  } else {
    Serial.print("FAIL (");
    Serial.print(with_high_cancel);
    Serial.print(" not < ");
    Serial.print(with_low_cancel);
    Serial.println(")");
    failed++;
  }

  // Reset
  apds.setProxCancellation(0);
}

void testAnalogCancellationEffect() {
  Serial.println("\n=== FUNCTIONAL TEST: Analog Cancellation Effect ===\n");

  // Paper should still be in position
  Serial.println("Paper in front of sensor...");

  // Reset cancellation
  apds.setProxCancellation(0);
  apds.setProxAnalogCancellation(0);
  delay(100);

  // Get baseline reading
  uint16_t baseline = getAvgProx(5);
  Serial.print("Baseline (analog=0): ");
  Serial.println(baseline);

  // Try analog cancellation level 2
  apds.setProxAnalogCancellation(2);
  delay(100);
  uint16_t with_analog_2 = getAvgProx(5);
  Serial.print("With analog=2: ");
  Serial.println(with_analog_2);

  // Try analog cancellation level 5
  apds.setProxAnalogCancellation(5);
  delay(100);
  uint16_t with_analog_5 = getAvgProx(5);
  Serial.print("With analog=5: ");
  Serial.println(with_analog_5);

  // Analog cancellation should reduce readings
  // The effect may be subtle, so we test that level 5 < baseline
  Serial.print("Analog cancellation has effect: ");
  if (with_analog_5 < baseline) {
    Serial.print("PASS (analog=5 ");
    Serial.print(with_analog_5);
    Serial.print(" < baseline ");
    Serial.print(baseline);
    Serial.println(")");
    passed++;
  } else if (with_analog_2 < baseline) {
    Serial.print("PASS (analog=2 ");
    Serial.print(with_analog_2);
    Serial.print(" < baseline ");
    Serial.print(baseline);
    Serial.println(")");
    passed++;
  } else {
    Serial.print("FAIL (no reduction seen, baseline=");
    Serial.print(baseline);
    Serial.print(", a2=");
    Serial.print(with_analog_2);
    Serial.print(", a5=");
    Serial.print(with_analog_5);
    Serial.println(")");
    failed++;
  }

  // Reset and move paper away
  apds.setProxAnalogCancellation(0);
  moveServo(SERVO_PAPER_OUT);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Cancellation Test");
  Serial.println("--------------------------");

  // Initialize servo
  servo.attach(SERVO_PIN);
  moveServo(SERVO_PAPER_OUT); // Start with paper away

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");
  apds.enableProximitySensor(true);
  delay(100);

  // Register read/write tests
  testDigitalCancellation();
  testAnalogCancellation();

  // Functional tests - verify settings actually affect readings
  testDigitalCancellationEffect();
  testAnalogCancellationEffect();

  Serial.println();
  Serial.print("Passed: ");
  Serial.print(passed);
  Serial.print(" / Failed: ");
  Serial.println(failed);
  Serial.println(failed == 0 ? "*** TEST PASSED ***" : "*** TEST FAILED ***");

  // Leave servo in known state
  moveServo(SERVO_PAPER_OUT);
}

void loop() {
  delay(1000);
}
