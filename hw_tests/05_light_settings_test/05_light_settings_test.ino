/*!
 * @file 05_light_settings_test.ino
 * @brief Test all resolution (20bit→13bit) and rate (25ms→2000ms) combos
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

struct ResolutionStep {
  apds9999_light_resolution_t res;
  const char* label;
};

struct RateStep {
  apds9999_light_meas_rate_t rate;
  const char* label;
};

const ResolutionStep res_steps[] = {
    {APDS9999_LIGHT_RES_20BIT, "20bit"}, {APDS9999_LIGHT_RES_19BIT, "19bit"},
    {APDS9999_LIGHT_RES_18BIT, "18bit"}, {APDS9999_LIGHT_RES_17BIT, "17bit"},
    {APDS9999_LIGHT_RES_16BIT, "16bit"}, {APDS9999_LIGHT_RES_13BIT, "13bit"}};

const RateStep rate_steps[] = {{APDS9999_LIGHT_RATE_25MS, "25ms"},
                               {APDS9999_LIGHT_RATE_50MS, "50ms"},
                               {APDS9999_LIGHT_RATE_100MS, "100ms"},
                               {APDS9999_LIGHT_RATE_200MS, "200ms"},
                               {APDS9999_LIGHT_RATE_500MS, "500ms"},
                               {APDS9999_LIGHT_RATE_1000MS, "1000ms"},
                               {APDS9999_LIGHT_RATE_2000MS, "2000ms"}};

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

  Serial.println("\nTesting resolution settings...\n");

  // Test all resolutions
  for (size_t i = 0; i < sizeof(res_steps) / sizeof(res_steps[0]); i++) {
    apds.setLightResolution(res_steps[i].res);
    apds9999_light_resolution_t readback = apds.getLightResolution();

    Serial.print("Resolution ");
    Serial.print(res_steps[i].label);
    Serial.print(": ");
    if (readback == res_steps[i].res) {
      Serial.println("PASS");
      passed++;
    } else {
      Serial.print("FAIL (got ");
      Serial.print(readback);
      Serial.println(")");
      failed++;
    }
  }

  Serial.println("\nTesting measurement rate settings...\n");

  // Test all rates
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
