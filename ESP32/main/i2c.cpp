#include "i2c.hpp"
#include <Wire.h>
#include <Arduino.h>

void onRequest() {
  const char* message = "Hello Pi!";
  Wire.write((uint8_t*)message, strlen(message));
}

void onReceive(int numBytes) {
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print(c);
  }
  Serial.println();
}
