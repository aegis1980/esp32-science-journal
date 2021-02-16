
#include <Arduino.h>

#include "debug_print.h"
#include "config_change.h"
#include <Esp32BLE.h>

extern PinType pin_type;
extern int pin;
int interval = 1000; // how often we pin via serial
long previousMillis = 0;

Esp32BLE mDevice;


void setup() {
  mDevice.init();
}

void loop() {
  unsigned long currentMillis = millis();
  uint32_t sensorValue = 0;

  if (mDevice.connected) {
    if (pin_type == P_ANALOG) {
      sensorValue = analogRead(pin);
    } else if (pin_type == P_DIGITAL) {
      sensorValue = digitalRead(pin);
    } else {
      sensorValue = 666;
    }

    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      if (pin_type == P_ANALOG) {
        DEBUG_PRINT("Analog ");
      } else if (pin_type == P_DIGITAL) {
        DEBUG_PRINT("Digital ");
      } else {
        DEBUG_PRINT("Unknown ");
      }

      DEBUG_PRINT("Pin: ");
      DEBUG_PRINT(pin);
      DEBUG_PRINT(" SensorValue: ");
      DEBUG_PRINTLN(sensorValue);
    }
     mDevice.sendValue(sensorValue);
  } else {
    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      DEBUG_PRINT("."); // just to let us know its alive via serial console
    }
  }
}