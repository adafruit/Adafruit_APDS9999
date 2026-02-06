/*!
 * @file 06_prox_settings_test.ino
 * @brief Test all prox resolution (8-11bit) and rate (6ms→400ms) combos
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

struct ResolutionStep {
  apds9999_prox_resolution_t res;
  const char* label;
};

struct RateStep {
  apds9999_prox_meas_rate_t rate;
  const char* label;
};

const ResolutionStep res_steps[] = {{APDS9999_PROX_RES_8BIT, "8bit"},
                                    {APDS9999_PROX_RES_9BIT, "9bit"},
                                    {APDS9999_PROX_RES_10BIT, "10bit"},
                                    {APDS9999_PROX_RES_11BIT, "11bit"}};

const RateStep rate_steps[] = {
    {APDS9999_PROX_RATE_6MS, "6ms"},     {APDS9999_PROX_RATE_12MS, "12ms"},
    {APDS9999_PROX_RATE_25MS, "25ms"},   {APDS9999_PROX_RATE_50MS, "50ms"},
    {APDS9999_PROX_RATE_100MS, "100ms"}, {APDS9999_PROX_RATE_200MS, "200ms"},
    {APDS9999_PROX_RATE_400MS, "400ms"}};

int passed = 0;
int failed = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Proximity Settings Test");
  Serial.println("---------------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");
  apds.enableProximitySensor(true);

  Serial.println("\nTesting resolution settings...\n");

  // Test all resolutions
  for (size_t i = 0; i < sizeof(res_steps) / sizeof(res_steps[0]); i++) {
    apds.setProxResolution(res_steps[i].res);
    apds9999_prox_resolution_t readback = apds.getProxResolution();

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
    apds.setProxMeasRate(rate_steps[i].rate);
    apds9999_prox_meas_rate_t readback = apds.getProxMeasRate();

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
