/*!
 * @file 04_light_gain_test.ino
 * @brief Cycle through all gain settings (1X, 3X, 6X, 9X, 18X)
 */

#include <Adafruit_APDS9999.h>

Adafruit_APDS9999 apds;

struct GainStep {
  apds9999_light_gain_t gain;
  const char* label;
  uint8_t multiplier;
};

const GainStep gain_steps[] = {{APDS9999_LIGHT_GAIN_1X, "1X", 1},
                               {APDS9999_LIGHT_GAIN_3X, "3X", 3},
                               {APDS9999_LIGHT_GAIN_6X, "6X", 6},
                               {APDS9999_LIGHT_GAIN_9X, "9X", 9},
                               {APDS9999_LIGHT_GAIN_18X, "18X", 18}};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Light Gain Test");
  Serial.println("------------------------");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable light sensor in RGB mode
  apds.enableLightSensor(true);
  apds.setRGBMode(true);

  Serial.println("\nCycling through gain settings...\n");
  Serial.println("Gain\tR\tG\tB\tIR");
  Serial.println("----\t-----\t-----\t-----\t-----");

  bool all_pass = true;

  for (size_t i = 0; i < sizeof(gain_steps) / sizeof(gain_steps[0]); i++) {
    // Set gain
    apds.setLightGain(gain_steps[i].gain);
    delay(300); // Wait for new reading

    // Verify gain was set correctly
    apds9999_light_gain_t readback = apds.getLightGain();
    if (readback != gain_steps[i].gain) {
      Serial.print("FAIL: Gain readback mismatch for ");
      Serial.println(gain_steps[i].label);
      all_pass = false;
      continue;
    }

    // Read values
    uint32_t r, g, b, ir;
    if (apds.getRGBIRData(&r, &g, &b, &ir)) {
      Serial.print(gain_steps[i].label);
      Serial.print("\t");
      Serial.print(r);
      Serial.print("\t");
      Serial.print(g);
      Serial.print("\t");
      Serial.print(b);
      Serial.print("\t");
      Serial.println(ir);
    } else {
      Serial.print(gain_steps[i].label);
      Serial.println("\tRead error!");
      all_pass = false;
    }
  }

  Serial.println();
  Serial.println(all_pass ? "*** TEST PASSED ***" : "*** TEST FAILED ***");
}

void loop() {
  delay(1000);
}
