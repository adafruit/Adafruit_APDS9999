/*!
 * @file 07_led_config_test.ino
 * @brief Test LED pulses, current, and frequency settings
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

int passed = 0;
int failed = 0;

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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 LED Config Test");
  Serial.println("------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");
  apds.enableProximitySensor(true);

  testPulses();
  testCurrent();
  testFrequency();

  Serial.println();
  Serial.print("Passed: ");
  Serial.print(passed);
  Serial.print(" / Failed: ");
  Serial.println(failed);
  Serial.println(failed == 0 ? "*** TEST PASSED ***" : "*** TEST FAILED ***");
}

void loop() {
  delay(1000);
}
