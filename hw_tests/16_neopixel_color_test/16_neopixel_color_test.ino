/*!
 * @file 16_neopixel_color_test.ino
 * @brief Read RGB from sensor, mirror color to NeoPixel ring
 * @note NeoPixel ring connected to D6, 16 pixels
 */

#include <Adafruit_APDS9999.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_COUNT 16

Adafruit_APDS9999 apds;
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 NeoPixel Color Test");
  Serial.println("----------------------------");
  Serial.println("NeoPixel ring on D6 (16 pixels)");

  // Initialize NeoPixels
  pixels.begin();
  pixels.setBrightness(50);                 // Not too bright
  pixels.fill(pixels.Color(255, 255, 255)); // White startup
  pixels.show();

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    // Show red on NeoPixels to indicate error
    pixels.fill(pixels.Color(255, 0, 0));
    pixels.show();
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!");

  // Enable light sensor in RGB mode with moderate gain
  apds.enableLightSensor(true);
  apds.setRGBMode(true);
  apds.setLightGain(APDS9999_LIGHT_GAIN_3X);
  apds.setLightResolution(APDS9999_LIGHT_RES_16BIT);
  apds.setLightMeasRate(APDS9999_LIGHT_RATE_50MS);

  // Flash green to indicate ready
  pixels.fill(pixels.Color(0, 255, 0));
  pixels.show();
  delay(500);

  Serial.println("\nHold colored objects near sensor!");
  Serial.println("NeoPixel ring will mirror the detected color.\n");
}

void loop() {
  uint32_t r, g, b, ir;

  if (apds.getRGBIRData(&r, &g, &b, &ir)) {
    // Find max value for normalization
    uint32_t maxVal = max(max(r, g), b);

    // Avoid division by zero and very dim readings
    if (maxVal < 100) {
      // Too dark - show dim white
      pixels.fill(pixels.Color(10, 10, 10));
      pixels.show();
      Serial.println("(too dark)");
      delay(50);
      return;
    }

    // Normalize to 0-255 range
    uint8_t r8 = (uint8_t)((r * 255UL) / maxVal);
    uint8_t g8 = (uint8_t)((g * 255UL) / maxVal);
    uint8_t b8 = (uint8_t)((b * 255UL) / maxVal);

    // Calculate brightness from overall light level
    // Use log scale for better perception
    uint8_t brightness = 50;
    if (maxVal > 10000) {
      brightness = 100;
    }
    if (maxVal > 50000) {
      brightness = 150;
    }
    if (maxVal > 100000) {
      brightness = 200;
    }

    pixels.setBrightness(brightness);

    // Set all pixels to the detected color
    uint32_t color = pixels.Color(r8, g8, b8);
    pixels.fill(color);
    pixels.show();

    // Print values
    Serial.print("R: ");
    Serial.print(r);
    Serial.print("\tG: ");
    Serial.print(g);
    Serial.print("\tB: ");
    Serial.print(b);
    Serial.print("\t-> RGB8(");
    Serial.print(r8);
    Serial.print(",");
    Serial.print(g8);
    Serial.print(",");
    Serial.print(b8);
    Serial.println(")");
  }

  delay(50);
}
