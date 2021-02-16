/*
 *  Copyright 2016 Google Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

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
  wait_for_serial();
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Science Journal ESP32 Startup");

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