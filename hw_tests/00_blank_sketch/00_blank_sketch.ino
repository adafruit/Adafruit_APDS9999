/*!
 * @file 00_blank_sketch.ino
 * @brief Empty sketch to silence board after tests
 * 
 * Upload this after running tests that use servos, NeoPixels, buzzers,
 * or other annoying actuators to stop ongoing noise/movement.
 */

void setup() {
  // Nothing to set up
}

void loop() {
  delay(1000);
}
