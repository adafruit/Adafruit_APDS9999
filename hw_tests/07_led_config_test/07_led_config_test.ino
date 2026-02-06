/*!
 * @file 07_led_config_test.ino
 * @brief Test LED pulses, current, and frequency settings
 *
 * Uses servo to position paper for consistent functional testing.
 * Verifies that LED settings actually affect proximity readings.
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

void testPulses() {
  Serial.println("Testing LED pulses (1, 64, 128, 255)...\n");

  uint8_t test_pulses[] = {1, 64, 128, 255};
  for (size_t i = 0; i < sizeof(test_pulses); i++) {
    apds.setLEDPulses(test_pulses[i]);
    uint8_t readback = apds.getLEDPulses();

    Serial.print("Pulses ");
    Serial.print(test_pulses[i]);
    Serial.print(": ");
    if (readback == test_pulses[i]) {
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

void testCurrent() {
  Serial.println("\nTesting LED current...\n");

  struct CurrentStep {
    apds9999_led_current_t current;
    const char* label;
  };

  CurrentStep steps[] = {{APDS9999_LED_CURRENT_10MA, "10mA"},
                         {APDS9999_LED_CURRENT_25MA, "25mA"}};

  for (size_t i = 0; i < sizeof(steps) / sizeof(steps[0]); i++) {
    apds.setLEDCurrent(steps[i].current);
    apds9999_led_current_t readback = apds.getLEDCurrent();

    Serial.print("Current ");
    Serial.print(steps[i].label);
    Serial.print(": ");
    if (readback == steps[i].current) {
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

void testFrequency() {
  Serial.println("\nTesting LED frequency...\n");

  struct FreqStep {
    apds9999_led_freq_t freq;
    const char* label;
  };

  FreqStep steps[] = {{APDS9999_LED_FREQ_60KHZ, "60kHz"},
                      {APDS9999_LED_FREQ_70KHZ, "70kHz"},
                      {APDS9999_LED_FREQ_80KHZ, "80kHz"},
                      {APDS9999_LED_FREQ_90KHZ, "90kHz"},
                      {APDS9999_LED_FREQ_100KHZ, "100kHz"}};

  for (size_t i = 0; i < sizeof(steps) / sizeof(steps[0]); i++) {
    apds.setLEDFrequency(steps[i].freq);
    apds9999_led_freq_t readback = apds.getLEDFrequency();

    Serial.print("Frequency ");
    Serial.print(steps[i].label);
    Serial.print(": ");
    if (readback == steps[i].freq) {
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

void testCurrentAffectsReadings() {
  Serial.println("\n=== FUNCTIONAL TEST: LED Current Affects Readings ===\n");

  // Position paper in front of sensor
  Serial.println("Moving paper in front of sensor...");
  moveServo(SERVO_PAPER_IN);
  delay(300);

  // Use moderate pulses and default frequency
  apds.setLEDPulses(64);
  apds.setLEDFrequency(APDS9999_LED_FREQ_100KHZ);

  // Test with low current (10mA)
  apds.setLEDCurrent(APDS9999_LED_CURRENT_10MA);
  delay(100);
  uint16_t prox_10mA = getAvgProx(5);
  Serial.print("Prox with 10mA current: ");
  Serial.println(prox_10mA);

  // Test with high current (25mA)
  apds.setLEDCurrent(APDS9999_LED_CURRENT_25MA);
  delay(100);
  uint16_t prox_25mA = getAvgProx(5);
  Serial.print("Prox with 25mA current: ");
  Serial.println(prox_25mA);

  // Higher current should produce higher prox readings
  Serial.print("Current affects readings: ");
  if (prox_25mA > prox_10mA) {
    Serial.print("PASS (25mA ");
    Serial.print(prox_25mA);
    Serial.print(" > 10mA ");
    Serial.print(prox_10mA);
    Serial.println(")");
    passed++;
  } else {
    Serial.print("FAIL (25mA ");
    Serial.print(prox_25mA);
    Serial.print(" not > 10mA ");
    Serial.print(prox_10mA);
    Serial.println(")");
    failed++;
  }

  // Move paper away
  moveServo(SERVO_PAPER_OUT);
}

void testPulsesAffectReadings() {
  Serial.println("\n=== FUNCTIONAL TEST: LED Pulses Affect Readings ===\n");

  // Position paper in front of sensor
  Serial.println("Moving paper in front of sensor...");
  moveServo(SERVO_PAPER_IN);
  delay(300);

  // Use standard current and frequency
  apds.setLEDCurrent(APDS9999_LED_CURRENT_10MA);
  apds.setLEDFrequency(APDS9999_LED_FREQ_100KHZ);

  // Test with few pulses
  apds.setLEDPulses(4);
  delay(100);
  uint16_t prox_4pulse = getAvgProx(5);
  Serial.print("Prox with 4 pulses: ");
  Serial.println(prox_4pulse);

  // Test with many pulses
  apds.setLEDPulses(64);
  delay(100);
  uint16_t prox_64pulse = getAvgProx(5);
  Serial.print("Prox with 64 pulses: ");
  Serial.println(prox_64pulse);

  // More pulses should produce higher prox readings (more integration)
  Serial.print("Pulses affect readings: ");
  if (prox_64pulse > prox_4pulse) {
    Serial.print("PASS (64 pulses ");
    Serial.print(prox_64pulse);
    Serial.print(" > 4 pulses ");
    Serial.print(prox_4pulse);
    Serial.println(")");
    passed++;
  } else {
    Serial.print("FAIL (64 pulses ");
    Serial.print(prox_64pulse);
    Serial.print(" not > 4 pulses ");
    Serial.print(prox_4pulse);
    Serial.println(")");
    failed++;
  }

  // Move paper away
  moveServo(SERVO_PAPER_OUT);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 LED Config Test");
  Serial.println("------------------------");

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

  // Register read/write tests
  testPulses();
  testCurrent();
  testFrequency();

  // Functional tests - verify settings actually affect readings
  testCurrentAffectsReadings();
  testPulsesAffectReadings();

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
