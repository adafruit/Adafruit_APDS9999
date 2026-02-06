/*!
 * @file 17_prox_logic_mode_test.ino
 * @brief Test proximity logic mode (INT pin behavior)
 * @note Connect INT pin to D2
 *
 * Logic Mode OFF: INT goes LOW on threshold crossing, stays until cleared
 * Logic Mode ON: INT follows proximity state (HIGH=object near, LOW=far)
 */

#include <Adafruit_APDS9999.h>

#define INT_PIN 2

Adafruit_APDS9999 apds;

volatile uint8_t intCount = 0;
volatile bool lastIntState = HIGH;

void isr() {
  intCount++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("APDS9999 Proximity Logic Mode Test");
  Serial.println("===================================");
  Serial.println("Connect sensor INT pin to D2\n");

  if (!apds.begin()) {
    Serial.println("FAIL: Could not find APDS9999 sensor!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("APDS9999 found!\n");

  // Test 1: Verify getter/setter for logic mode
  Serial.println("TEST 1: Logic Mode Getter/Setter");
  Serial.println("--------------------------------");

  apds.setProxLogicMode(false);
  bool mode = apds.getProxLogicMode();
  Serial.print("setProxLogicMode(false) -> getProxLogicMode() = ");
  Serial.println(mode ? "true (FAIL)" : "false (PASS)");

  apds.setProxLogicMode(true);
  mode = apds.getProxLogicMode();
  Serial.print("setProxLogicMode(true) -> getProxLogicMode() = ");
  Serial.println(mode ? "true (PASS)" : "false (FAIL)");

  // Enable proximity sensor
  apds.enableProximitySensor(true);

  // Set threshold - interrupt when prox > 200
  apds.setProxThresholdLow(0);
  apds.setProxThresholdHigh(200);
  apds.setProxPersistence(1); // Fast response

  // Enable proximity interrupt
  apds.enableProxInterrupt(true);

  // Setup interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);

  Serial.println("\n\nTEST 2: Logic Mode OFF (Normal Interrupt)");
  Serial.println("-----------------------------------------");
  Serial.println("In this mode, INT goes LOW when threshold crossed,");
  Serial.println("stays LOW until status is read to clear.\n");

  apds.setProxLogicMode(false);
  apds.getMainStatus(); // Clear any pending

  intCount = 0;
  attachInterrupt(digitalPinToInterrupt(INT_PIN), isr, CHANGE);

  Serial.println("Wave hand near sensor... watching for 5 seconds:");
  runTest(5000);

  Serial.println("\n\nTEST 3: Logic Mode ON (Logic Signal Output)");
  Serial.println("-------------------------------------------");
  Serial.println("In this mode, INT follows logic state directly:");
  Serial.println("  - HIGH when prox < threshold (no object)");
  Serial.println("  - LOW when prox > threshold (object near)\n");

  apds.setProxLogicMode(true);
  apds.getMainStatus(); // Clear status

  intCount = 0;

  Serial.println("Wave hand near sensor... watching for 5 seconds:");
  runTest(5000);

  Serial.println("\n\nTEST COMPLETE");
  Serial.println("=============");
  Serial.println("Logic mode OFF: INT latches on threshold cross");
  Serial.println("Logic mode ON: INT tracks object presence in real-time");
}

void runTest(uint16_t durationMs) {
  unsigned long start = millis();
  unsigned long lastPrint = 0;
  uint8_t lastCount = 0;

  while (millis() - start < durationMs) {
    uint16_t prox;
    apds.readProximity(&prox);

    bool intPin = digitalRead(INT_PIN);
    uint8_t status = apds.getMainStatus();
    bool logicBit = (status & APDS9999_STATUS_PROX_LOGIC) ? true : false;

    // Print every 200ms
    if (millis() - lastPrint >= 200) {
      lastPrint = millis();

      Serial.print("Prox: ");
      if (prox < 100)
        Serial.print("  ");
      else if (prox < 1000)
        Serial.print(" ");
      Serial.print(prox);

      Serial.print(" | INT pin: ");
      Serial.print(intPin ? "HIGH" : "LOW ");

      Serial.print(" | Logic bit: ");
      Serial.print(logicBit ? "1" : "0");

      Serial.print(" | INT changes: ");
      Serial.print(intCount);

      if (intCount > lastCount) {
        Serial.print(" <-- edge detected!");
        lastCount = intCount;
      }

      Serial.println();
    }

    delay(50);
  }
}

void loop() {
  // Test runs once in setup
  delay(1000);
}
