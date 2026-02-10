/*!
 * @file 05_light_settings_test.ino
 * @brief Test resolution settings with actual readings, rate setter/getter,
 *        and measurement rate timing verification
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

struct ResolutionStep {
  apds9999_light_resolution_t res;
  const char* label;
  uint16_t delay_ms; // Conversion time for this resolution
};

struct RateStep {
  apds9999_light_meas_rate_t rate;
  const char* label;
  uint16_t expected_ms; // Expected measurement period in ms
};

// Delays based on typical conversion times per resolution
const ResolutionStep res_steps[] = {{APDS9999_LIGHT_RES_20BIT, "20bit", 400},
                                    {APDS9999_LIGHT_RES_19BIT, "19bit", 200},
                                    {APDS9999_LIGHT_RES_18BIT, "18bit", 100},
                                    {APDS9999_LIGHT_RES_17BIT, "17bit", 50},
                                    {APDS9999_LIGHT_RES_16BIT, "16bit", 25},
                                    {APDS9999_LIGHT_RES_13BIT, "13bit", 13}};

const RateStep rate_steps[] = {{APDS9999_LIGHT_RATE_25MS, "25ms", 25},
                               {APDS9999_LIGHT_RATE_50MS, "50ms", 50},
                               {APDS9999_LIGHT_RATE_100MS, "100ms", 100},
                               {APDS9999_LIGHT_RATE_200MS, "200ms", 200},
                               {APDS9999_LIGHT_RATE_500MS, "500ms", 500},
                               {APDS9999_LIGHT_RATE_1000MS, "1000ms", 1000},
                               {APDS9999_LIGHT_RATE_2000MS, "2000ms", 2000}};

int passed = 0;
int failed = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Light Settings Test");
  Serial.println("----------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");
  apds.enableLightSensor(true);
  apds.setRGBMode(true);

  Serial.println("\nTesting resolution settings with readings...\n");
  Serial.println("Resolution\tGreen\tSet/Get");
  Serial.println("----------\t-----\t-------");

  // Test all resolutions with actual readings
  for (size_t i = 0; i < sizeof(res_steps) / sizeof(res_steps[0]); i++) {
    apds.setLightResolution(res_steps[i].res);
    delay(res_steps[i].delay_ms + 50); // Wait for conversion + margin

    apds9999_light_resolution_t readback = apds.getLightResolution();
    bool setget_pass = (readback == res_steps[i].res);

    // Read green value
    uint32_t r, g, b, ir;
    bool read_ok = apds.getRGBIRData(&r, &g, &b, &ir);

    Serial.print(res_steps[i].label);
    Serial.print("\t\t");
    if (read_ok) {
      Serial.print(g);
    } else {
      Serial.print("ERR");
    }
    Serial.print("\t");
    Serial.println(setget_pass ? "PASS" : "FAIL");

    if (setget_pass) {
      passed++;
    } else {
      failed++;
    }
  }

  Serial.println("\nTesting measurement rate settings...\n");

  // Test all rates (setter/getter only)
  for (size_t i = 0; i < sizeof(rate_steps) / sizeof(rate_steps[0]); i++) {
    apds.setLightMeasRate(rate_steps[i].rate);
    apds9999_light_meas_rate_t readback = apds.getLightMeasRate();

    Serial.print("Rate ");
    Serial.print(rate_steps[i].label);
    Serial.print(": ");
    if (readback == rate_steps[i].rate) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }

  // Timing test section
  Serial.println("\nTesting measurement rate timing...\n");
  Serial.println("Rate\t\tExpected\tActual\tStatus");
  Serial.println("----\t\t--------\t------\t------");

  // Use fastest resolution (13bit, 3.125ms) so resolution doesn't limit rate
  apds.setLightResolution(APDS9999_LIGHT_RES_13BIT);
  delay(50);

  for (size_t i = 0; i < sizeof(rate_steps) / sizeof(rate_steps[0]); i++) {
    // Set the rate
    apds.setLightMeasRate(rate_steps[i].rate);
    delay(10);

    // Clear any pending data by reading
    uint32_t r, g, b, ir;
    apds.getRGBIRData(&r, &g, &b, &ir);

    // Wait for first data ready
    unsigned long timeout = millis() + rate_steps[i].expected_ms * 3;
    while (!apds.isLightDataReady()) {
      if (millis() > timeout)
        break;
    }

    // Read to clear ready flag
    apds.getRGBIRData(&r, &g, &b, &ir);

    // Record start time
    unsigned long start_time = millis();

    // Wait for next data ready
    timeout = millis() + rate_steps[i].expected_ms * 3;
    while (!apds.isLightDataReady()) {
      if (millis() > timeout)
        break;
    }

    // Record end time
    unsigned long end_time = millis();
    unsigned long actual_ms = end_time - start_time;

    // Check if within 20% tolerance
    uint16_t expected = rate_steps[i].expected_ms;
    uint16_t tolerance = expected / 5; // 20%
    bool timing_pass = (actual_ms >= expected - tolerance) &&
                       (actual_ms <= expected + tolerance);

    // Print results
    Serial.print(rate_steps[i].label);
    Serial.print("\t\t");
    Serial.print(expected);
    Serial.print("\t\t");
    Serial.print(actual_ms);
    Serial.print("\t");
    Serial.println(timing_pass ? "PASS" : "FAIL");

    if (timing_pass) {
      passed++;
    } else {
      failed++;
    }
  }

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
