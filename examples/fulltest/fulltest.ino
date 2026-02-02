/*
 * Full test sketch for APDS-9999 Digital Proximity and RGB Sensor
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 *
 * Tests all sensor functions and displays continuous data from both
 * proximity and light sensors running simultaneously.
 */

#include "Adafruit_APDS9999.h"

Adafruit_APDS9999 apds = Adafruit_APDS9999();

#define INT_PIN 2

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println(F("APDS-9999 Full Test"));
  Serial.println(F("==================="));

  if (!apds.begin()) {
    Serial.println(F("Couldn't find APDS-9999 chip"));
    while (1)
      delay(10);
  }
  
  // Print chip info
  Serial.print(F("Part ID: 0x"));
  Serial.print(apds.getPartID(), HEX);
  Serial.print(F(" Rev: 0x"));
  Serial.println(apds.getRevisionID(), HEX);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);

  // === Light Sensor Configuration ===
  Serial.println(F("\n--- Light Sensor Config ---"));
  
  apds.enableLightSensor(true);
  Serial.print(F("LS Enabled: "));
  Serial.println(apds.lightSensorEnabled() ? F("Yes") : F("No"));

  apds.setRGBMode(true);
  Serial.print(F("RGB Mode: "));
  Serial.println(apds.getRGBMode() ? F("RGB+IR") : F("ALS+IR only"));

  // Set and display gain
  apds.setLSGain(APDS9999_LS_GAIN_3X);
  Serial.print(F("LS Gain: "));
  switch (apds.getLSGain()) {
    case APDS9999_LS_GAIN_1X:  Serial.println(F("1x")); break;
    case APDS9999_LS_GAIN_3X:  Serial.println(F("3x")); break;
    case APDS9999_LS_GAIN_6X:  Serial.println(F("6x")); break;
    case APDS9999_LS_GAIN_9X:  Serial.println(F("9x")); break;
    case APDS9999_LS_GAIN_18X: Serial.println(F("18x")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // Set and display resolution
  apds.setLSResolution(APDS9999_LS_RES_18BIT);
  Serial.print(F("LS Resolution: "));
  switch (apds.getLSResolution()) {
    case APDS9999_LS_RES_20BIT: Serial.println(F("20-bit (400ms)")); break;
    case APDS9999_LS_RES_19BIT: Serial.println(F("19-bit (200ms)")); break;
    case APDS9999_LS_RES_18BIT: Serial.println(F("18-bit (100ms)")); break;
    case APDS9999_LS_RES_17BIT: Serial.println(F("17-bit (50ms)")); break;
    case APDS9999_LS_RES_16BIT: Serial.println(F("16-bit (25ms)")); break;
    case APDS9999_LS_RES_13BIT: Serial.println(F("13-bit (3ms)")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // Set and display measurement rate
  apds.setLSMeasRate(APDS9999_LS_RATE_100MS);
  Serial.print(F("LS Meas Rate: "));
  switch (apds.getLSMeasRate()) {
    case APDS9999_LS_RATE_25MS:   Serial.println(F("25ms")); break;
    case APDS9999_LS_RATE_50MS:   Serial.println(F("50ms")); break;
    case APDS9999_LS_RATE_100MS:  Serial.println(F("100ms")); break;
    case APDS9999_LS_RATE_200MS:  Serial.println(F("200ms")); break;
    case APDS9999_LS_RATE_500MS:  Serial.println(F("500ms")); break;
    case APDS9999_LS_RATE_1000MS: Serial.println(F("1000ms")); break;
    case APDS9999_LS_RATE_2000MS: Serial.println(F("2000ms")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // === Proximity Sensor Configuration ===
  Serial.println(F("\n--- Proximity Sensor Config ---"));

  apds.enableProximitySensor(true);
  Serial.print(F("PS Enabled: "));
  Serial.println(apds.proximitySensorEnabled() ? F("Yes") : F("No"));

  // Set and display resolution
  apds.setPSResolution(APDS9999_PS_RES_11BIT);
  Serial.print(F("PS Resolution: "));
  switch (apds.getPSResolution()) {
    case APDS9999_PS_RES_8BIT:  Serial.println(F("8-bit")); break;
    case APDS9999_PS_RES_9BIT:  Serial.println(F("9-bit")); break;
    case APDS9999_PS_RES_10BIT: Serial.println(F("10-bit")); break;
    case APDS9999_PS_RES_11BIT: Serial.println(F("11-bit")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // Set and display measurement rate
  apds.setPSMeasRate(APDS9999_PS_RATE_100MS);
  Serial.print(F("PS Meas Rate: "));
  switch (apds.getPSMeasRate()) {
    case APDS9999_PS_RATE_6MS:   Serial.println(F("6.25ms")); break;
    case APDS9999_PS_RATE_12MS:  Serial.println(F("12.5ms")); break;
    case APDS9999_PS_RATE_25MS:  Serial.println(F("25ms")); break;
    case APDS9999_PS_RATE_50MS:  Serial.println(F("50ms")); break;
    case APDS9999_PS_RATE_100MS: Serial.println(F("100ms")); break;
    case APDS9999_PS_RATE_200MS: Serial.println(F("200ms")); break;
    case APDS9999_PS_RATE_400MS: Serial.println(F("400ms")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // Set and display LED current
  apds.setLEDCurrent(APDS9999_LED_CURRENT_25MA);
  Serial.print(F("LED Current: "));
  switch (apds.getLEDCurrent()) {
    case APDS9999_LED_CURRENT_10MA: Serial.println(F("10mA")); break;
    case APDS9999_LED_CURRENT_25MA: Serial.println(F("25mA")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // Set and display LED frequency
  apds.setLEDFrequency(APDS9999_LED_FREQ_60KHZ);
  Serial.print(F("LED Frequency: "));
  switch (apds.getLEDFrequency()) {
    case APDS9999_LED_FREQ_60KHZ:  Serial.println(F("60kHz")); break;
    case APDS9999_LED_FREQ_70KHZ:  Serial.println(F("70kHz")); break;
    case APDS9999_LED_FREQ_80KHZ:  Serial.println(F("80kHz")); break;
    case APDS9999_LED_FREQ_90KHZ:  Serial.println(F("90kHz")); break;
    case APDS9999_LED_FREQ_100KHZ: Serial.println(F("100kHz")); break;
    default: Serial.println(F("Unknown")); break;
  }

  // Set and display LED pulses
  apds.setLEDPulses(8);
  Serial.print(F("LED Pulses: "));
  Serial.println(apds.getLEDPulses());

  // === Interrupt Configuration ===
  Serial.println(F("\n--- Interrupt Config ---"));

  apds.setPSThresholdLow(0);
  apds.setPSThresholdHigh(100);
  Serial.print(F("PS Thresholds: Low="));
  Serial.print(apds.getPSThresholdLow());
  Serial.print(F(" High="));
  Serial.println(apds.getPSThresholdHigh());

  apds.setPSPersistence(1);
  Serial.print(F("PS Persistence: "));
  Serial.println(apds.getPSPersistence());

  apds.enablePSInterrupt(true);
  Serial.print(F("PS Interrupt: "));
  Serial.println(apds.psInterruptEnabled() ? F("Enabled") : F("Disabled"));

  // === Ready for continuous reading ===
  Serial.println(F("\n--- Starting Continuous Read ---"));
  Serial.println(F("Format: [Flags] Prox | R G B IR"));
  Serial.println(F("Flags: P=PS_RDY L=LS_RDY I=INT_PIN i=PS_INT"));
  Serial.println();
  
  delay(500);  // Let sensors stabilize
}

void loop() {
  // Read status flags
  bool ps_rdy = false;  // Would need isPSDataReady() if we add it
  bool ls_rdy = false;  // Would need isLSDataReady() if we add it
  bool int_pin = (digitalRead(INT_PIN) == LOW);
  bool ps_int = apds.getPSInterruptStatus();  // Note: reading clears the flag

  // Print flags
  Serial.print(F("["));
  Serial.print(int_pin ? F("I") : F("-"));
  Serial.print(ps_int ? F("i") : F("-"));
  Serial.print(F("] "));

  // Read proximity
  uint16_t prox = apds.readProximity();
  bool overflow = apds.getProximityOverflow();
  
  Serial.print(F("Prox:"));
  if (overflow) {
    Serial.print(F("OVF"));
  } else {
    Serial.print(prox);
  }
  
  // Read RGBIR
  uint32_t r, g, b, ir;
  if (apds.getRGBIRData(&r, &g, &b, &ir)) {
    Serial.print(F(" | R:"));
    Serial.print(r);
    Serial.print(F(" G:"));
    Serial.print(g);
    Serial.print(F(" B:"));
    Serial.print(b);
    Serial.print(F(" IR:"));
    Serial.print(ir);
  } else {
    Serial.print(F(" | RGB read failed"));
  }

  Serial.println();

  // Check for serial commands
  if (Serial.available()) {
    char c = Serial.read();
    handleCommand(c);
  }

  delay(100);
}

void handleCommand(char cmd) {
  switch (cmd) {
    case 'g':  // Cycle LS gain
      {
        apds9999_ls_gain_t gain = apds.getLSGain();
        if (gain < APDS9999_LS_GAIN_18X) {
          gain = (apds9999_ls_gain_t)(gain + 1);
        } else {
          gain = APDS9999_LS_GAIN_1X;
        }
        apds.setLSGain(gain);
        Serial.print(F("LS Gain -> "));
        Serial.println(gain);
      }
      break;

    case 'r':  // Cycle LS resolution
      {
        apds9999_ls_resolution_t res = apds.getLSResolution();
        if (res < APDS9999_LS_RES_13BIT) {
          res = (apds9999_ls_resolution_t)(res + 1);
        } else {
          res = APDS9999_LS_RES_20BIT;
        }
        apds.setLSResolution(res);
        Serial.print(F("LS Res -> "));
        Serial.println(res);
      }
      break;

    case 'p':  // Toggle proximity sensor
      {
        bool en = !apds.proximitySensorEnabled();
        apds.enableProximitySensor(en);
        Serial.print(F("PS -> "));
        Serial.println(en ? F("ON") : F("OFF"));
      }
      break;

    case 'l':  // Toggle light sensor
      {
        bool en = !apds.lightSensorEnabled();
        apds.enableLightSensor(en);
        Serial.print(F("LS -> "));
        Serial.println(en ? F("ON") : F("OFF"));
      }
      break;

    case 'i':  // Toggle PS interrupt
      {
        bool en = !apds.psInterruptEnabled();
        apds.enablePSInterrupt(en);
        Serial.print(F("PS Int -> "));
        Serial.println(en ? F("ON") : F("OFF"));
      }
      break;

    case '+':  // Increase PS threshold high
      {
        uint16_t t = apds.getPSThresholdHigh() + 50;
        if (t > 2047) t = 2047;
        apds.setPSThresholdHigh(t);
        Serial.print(F("PS Thresh High -> "));
        Serial.println(t);
      }
      break;

    case '-':  // Decrease PS threshold high
      {
        int16_t t = apds.getPSThresholdHigh() - 50;
        if (t < 0) t = 0;
        apds.setPSThresholdHigh(t);
        Serial.print(F("PS Thresh High -> "));
        Serial.println(t);
      }
      break;

    case '?':  // Help
      Serial.println(F("\n--- Commands ---"));
      Serial.println(F("g - Cycle LS gain"));
      Serial.println(F("r - Cycle LS resolution"));
      Serial.println(F("p - Toggle proximity sensor"));
      Serial.println(F("l - Toggle light sensor"));
      Serial.println(F("i - Toggle PS interrupt"));
      Serial.println(F("+ - Increase PS threshold"));
      Serial.println(F("- - Decrease PS threshold"));
      Serial.println(F("? - This help"));
      Serial.println();
      break;

    default:
      break;
  }
}
