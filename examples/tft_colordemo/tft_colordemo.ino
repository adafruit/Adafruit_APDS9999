/*
 * APDS-9999 Proximity + Color Display Demo for ESP32-S2 TFT Feather
 *
 * Displays a proximity bar gauge and a live RGB color swatch on the
 * built-in 1.14" TFT.
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 */

#include <Adafruit_APDS9999.h>
#include <Adafruit_ST7789.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

Adafruit_APDS9999 apds;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);

const uint16_t SCREEN_W = 240;
const uint16_t SCREEN_H = 135;
const uint16_t TITLE_H = 17;
const uint16_t TOP_BAR_H = 25;
const uint16_t BOTTOM_H = SCREEN_H - TOP_BAR_H - TITLE_H;
const uint16_t LEFT_W = (SCREEN_W * 45) / 100;
const uint16_t RIGHT_W = SCREEN_W - LEFT_W;

const uint16_t PROX_BAR_X = 0;
const uint16_t PROX_BAR_Y = TITLE_H;
const uint16_t PROX_BAR_W = SCREEN_W;
const uint16_t PROX_BAR_H = TOP_BAR_H;
const uint16_t PROX_MAX = 2047;

const uint16_t SWATCH_MARGIN = 4;
const uint16_t SWATCH_X = SWATCH_MARGIN;
const uint16_t SWATCH_Y = PROX_BAR_Y + TOP_BAR_H + SWATCH_MARGIN;
const uint16_t SWATCH_W = LEFT_W - (SWATCH_MARGIN * 2);
const uint16_t SWATCH_H = BOTTOM_H - (SWATCH_MARGIN * 2);

void setup() {
  Serial.begin(115200);
  delay(100);

  // Turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(100);

  display.init(135, 240);
  display.setRotation(3);

  canvas.setFont(&FreeSans9pt7b);
  canvas.setTextColor(ST77XX_WHITE);

  if (!apds.begin()) {
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setCursor(0, 40);
    canvas.setTextColor(ST77XX_RED);
    canvas.println(" APDS-9999 not found!");
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
    while (1)
      delay(10);
  }

  // Configure sensor
  apds.enableLightSensor(true);
  apds.enableProximitySensor(true);
  apds.setRGBMode(true);
  apds.setLightResolution(APDS9999_LIGHT_RES_18BIT);
  apds.setLightMeasRate(APDS9999_LIGHT_RATE_100MS);
  apds.setLightGain(APDS9999_LIGHT_GAIN_6X);
  apds.setProxResolution(APDS9999_PROX_RES_11BIT);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
}

void loop() {
  uint16_t prox = 0;
  uint32_t red = 0, green = 0, blue = 0, ir = 0;

  apds.readProximity(&prox);
  apds.getRGBIRData(&red, &green, &blue, &ir);

  float lux = apds.calculateLux(green);

  // Apply per-channel correction to compensate for green sensitivity bias.
  // Green photodiodes have ~2x response; these factors normalize to white.
  float rc = red * 2.0;
  float gc = green * 1.0;
  float bc = blue * 1.6;

  float maxc = rc;
  if (gc > maxc)
    maxc = gc;
  if (bc > maxc)
    maxc = bc;

  uint8_t r8 = 0, g8 = 0, b8 = 0;
  if (maxc > 0) {
    r8 = min(255, (int)(rc * 255.0 / maxc));
    g8 = min(255, (int)(gc * 255.0 / maxc));
    b8 = min(255, (int)(bc * 255.0 / maxc));
  }

  uint16_t swatchColor = display.color565(r8, g8, b8);

  canvas.fillScreen(ST77XX_BLACK);

  canvas.setFont(&FreeSans9pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.setCursor(4, 13);
  canvas.print("Adafruit APDS-9999 Demo");

  // Proximity gauge (top bar)
  canvas.drawRect(PROX_BAR_X, PROX_BAR_Y, PROX_BAR_W, PROX_BAR_H,
                  ST77XX_WHITE);
  uint16_t fillw = (uint32_t)prox * (PROX_BAR_W - 2) / PROX_MAX;
  if (fillw > 0) {
    canvas.fillRect(PROX_BAR_X + 1, PROX_BAR_Y + 1, fillw, PROX_BAR_H - 2,
                    ST77XX_CYAN);
  }

  canvas.setFont(&FreeSans9pt7b);
  if (fillw > 60) {
    canvas.setTextColor(ST77XX_BLACK);
  } else {
    canvas.setTextColor(ST77XX_WHITE);
  }
  canvas.setCursor(6, PROX_BAR_Y + 18);
  canvas.print("PROX");

  char proxBuf[8];
  snprintf(proxBuf, sizeof(proxBuf), "%u", prox);
  int16_t bx, by;
  uint16_t bw, bh;
  canvas.getTextBounds(proxBuf, 0, 0, &bx, &by, &bw, &bh);
  if (fillw > (PROX_BAR_W - 60)) {
    canvas.setTextColor(ST77XX_BLACK);
  } else {
    canvas.setTextColor(ST77XX_WHITE);
  }
  canvas.setCursor(SCREEN_W - bw - 6, PROX_BAR_Y + 18);
  canvas.print(proxBuf);

  // Color swatch (left zone)
  canvas.drawRect(SWATCH_X - 1, SWATCH_Y - 1, SWATCH_W + 2, SWATCH_H + 2,
                  ST77XX_WHITE);
  canvas.fillRect(SWATCH_X, SWATCH_Y, SWATCH_W, SWATCH_H, swatchColor);

  // RGB/Lux readout (right zone)
  uint16_t textX = LEFT_W + 8;
  uint16_t textY = TITLE_H + TOP_BAR_H + 20;
  uint16_t lineStep = 24;
  canvas.setFont(&FreeSansBold12pt7b);

  canvas.setTextColor(ST77XX_RED);
  canvas.setCursor(textX, textY);
  canvas.print("R: ");
  canvas.println(red);

  canvas.setTextColor(ST77XX_GREEN);
  canvas.setCursor(textX, textY + lineStep);
  canvas.print("G: ");
  canvas.println(green);

  canvas.setTextColor(ST77XX_BLUE);
  canvas.setCursor(textX, textY + (lineStep * 2));
  canvas.print("B: ");
  canvas.println(blue);

  canvas.setTextColor(ST77XX_WHITE);
  canvas.setCursor(textX, textY + (lineStep * 3) - 1);
  canvas.print("Lux: ");
  canvas.println(lux, 1);

  display.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
  delay(200);
}
