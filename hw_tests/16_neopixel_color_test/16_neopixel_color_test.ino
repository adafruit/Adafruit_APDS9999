/*!
 * @file 16_neopixel_color_test.ino
 * @brief Verify RGB sensor correctly distinguishes R/G/B light from NeoPixel
 * @note NeoPixel ring connected to D6, 16 pixels
 */

#include <Adafruit_APDS9999.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16

Adafruit_APDS9999 apds;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

bool testA_pass = false;
bool testB_pass = false;
bool testC_pass = false;
bool testD_pass = false;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 NeoPixel RGB Color Test");
  Serial.println("================================");
  Serial.println("NeoPixel ring on D6 (16 pixels)");
  Serial.println();

  // Initialize NeoPixels
  pixels.begin();
  pixels.setBrightness(255); // Full brightness for reliable readings
  pixels.fill(0);
  pixels.show();

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    pixels.fill(pixels.Color(255, 0, 0));
    pixels.show();
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable light sensor in RGB mode with 3X gain
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);
  apds.setLightResolution(APDS9999_LIGHT_RES_16BIT);
  apds.setLightMeasRate(APDS9999_LIGHT_RATE_100MS);

  delay(200); // Let sensor settle

  // Run tests
  testA_pass = testRed();
  testB_pass = testGreen();
  testC_pass = testBlue();
  testD_pass = testWhiteVsOff();

  // Print summary
  Serial.println();
  Serial.println("================================");
  Serial.println("TEST SUMMARY:");
  Serial.print("Test A (RED):      ");
  Serial.println(testA_pass ? "PASS" : "FAIL");
  Serial.print("Test B (GREEN):    ");
  Serial.println(testB_pass ? "PASS" : "FAIL");
  Serial.print("Test C (BLUE):     ");
  Serial.println(testC_pass ? "PASS" : "FAIL");
  Serial.print("Test D (WHITE/OFF):");
  Serial.println(testD_pass ? "PASS" : "FAIL");
  Serial.println("================================");

  bool allPass = testA_pass && testB_pass && testC_pass && testD_pass;
  Serial.print("OVERALL: ");
  Serial.println(allPass ? "PASS" : "FAIL");

  // Cleanup - turn off NeoPixels
  pixels.fill(0);
  pixels.show();

  Serial.println("\nTest complete. NeoPixels off.");
}

void loop() {
  // Nothing to do
  delay(1000);
}

bool testRed() {
  Serial.println("\n--- Test A: RED Detection ---");

  // Set NeoPixel to RED
  pixels.fill(pixels.Color(255, 0, 0));
  pixels.show();
  delay(300);

  uint32_t r, g, b, ir;
  if (!apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.println("ERROR: Failed to read sensor data");
    return false;
  }

  Serial.print("R=");
  Serial.print(r);
  Serial.print(" G=");
  Serial.print(g);
  Serial.print(" B=");
  Serial.println(b);

  bool pass = (r > g) && (r > b);
  Serial.print("R highest? ");
  Serial.println(pass ? "PASS" : "FAIL");

  pixels.fill(0);
  pixels.show();
  delay(100);

  return pass;
}

bool testGreen() {
  Serial.println("\n--- Test B: GREEN Detection ---");

  // Set NeoPixel to GREEN
  pixels.fill(pixels.Color(0, 255, 0));
  pixels.show();
  delay(300);

  uint32_t r, g, b, ir;
  if (!apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.println("ERROR: Failed to read sensor data");
    return false;
  }

  Serial.print("R=");
  Serial.print(r);
  Serial.print(" G=");
  Serial.print(g);
  Serial.print(" B=");
  Serial.println(b);

  bool pass = (g > r) && (g > b);
  Serial.print("G highest? ");
  Serial.println(pass ? "PASS" : "FAIL");

  pixels.fill(0);
  pixels.show();
  delay(100);

  return pass;
}

bool testBlue() {
  Serial.println("\n--- Test C: BLUE Detection ---");

  // Set NeoPixel to BLUE
  pixels.fill(pixels.Color(0, 0, 255));
  pixels.show();
  delay(300);

  uint32_t r, g, b, ir;
  if (!apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.println("ERROR: Failed to read sensor data");
    return false;
  }

  Serial.print("R=");
  Serial.print(r);
  Serial.print(" G=");
  Serial.print(g);
  Serial.print(" B=");
  Serial.println(b);

  bool pass = (b > r) && (b > g);
  Serial.print("B highest? ");
  Serial.println(pass ? "PASS" : "FAIL");

  pixels.fill(0);
  pixels.show();
  delay(100);

  return pass;
}

bool testWhiteVsOff() {
  Serial.println("\n--- Test D: WHITE vs OFF ---");

  // Set NeoPixel to WHITE
  pixels.fill(pixels.Color(255, 255, 255));
  pixels.show();
  delay(300);

  uint32_t rW, gW, bW, irW;
  if (!apds.getRGBIRData(&rW, &gW, &bW, &irW)) {
    Serial.println("ERROR: Failed to read sensor data (WHITE)");
    return false;
  }

  Serial.print("WHITE: R=");
  Serial.print(rW);
  Serial.print(" G=");
  Serial.print(gW);
  Serial.print(" B=");
  Serial.println(bW);

  // Set NeoPixel OFF
  pixels.fill(0);
  pixels.show();
  delay(300);

  uint32_t rO, gO, bO, irO;
  if (!apds.getRGBIRData(&rO, &gO, &bO, &irO)) {
    Serial.println("ERROR: Failed to read sensor data (OFF)");
    return false;
  }

  Serial.print("OFF:   R=");
  Serial.print(rO);
  Serial.print(" G=");
  Serial.print(gO);
  Serial.print(" B=");
  Serial.println(bO);

  bool pass = (rW > rO) && (gW > gO) && (bW > bO);
  Serial.print("All channels higher with WHITE? ");
  Serial.println(pass ? "PASS" : "FAIL");

  return pass;
}
