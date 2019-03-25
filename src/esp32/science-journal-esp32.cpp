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

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "WiFi.h"

#include <pb.h>
#include "goosci_utility.h"
#include "debug_print.h"
#include "config_change.h"
#include "heartbeat.h"
#include "sensor.pb.h"

#define WHISTLEPUNK_SERVICE_UUID "555a0001-0aaa-467a-9538-01f0652c74e8"
BLECharacteristic* valueCharacteristic = NULL;

bool deviceConnected = false;
String BleLongName;
extern PinType pin_type;
extern int pin;
int interval = 1000; // how often we pin via serial
long previousMillis = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      DEBUG_PRINTLN("BLE Client Connected");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      DEBUG_PRINTLN("BLE Client Disconnected");
      deviceConnected = false;
    }
};

class ValueCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      DEBUG_PRINTLN("valueCharacteristic onWrite()");
    }

    void onRead(BLECharacteristic *pCharacteristic) {
      DEBUG_PRINTLN("valueCharacteristic onRead()");
    }
};

class VersionCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
       DEBUG_PRINTLN("versionCharacteristic onRead()");
    }
};

class ConfigCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      DEBUG_PRINTLN("configCharacteristic onWrite()");
      uint8_t* v = pCharacteristic->getData();
      handle(v, sizeof(v));
    }

    void onRead(BLECharacteristic *pCharacteristic) {
      DEBUG_PRINTLN("configCharacteristic onRead()");
    }
};

void setup() {
  wait_for_serial();
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Science Journal ESP32 Startup");

  // I can't seem to initialize the Bluetooth Radio and then change the broadcast
  // name, so we're going to use the WiFi MAC Address instead
  WiFi.mode(WIFI_MODE_STA);
  String address = WiFi.macAddress();
  WiFi.mode(WIFI_OFF);

  address.toUpperCase();
  DEBUG_PRINT("WiFi MAC Address: ");
  DEBUG_PRINTLN(address);
  BleLongName = "Sci";
  BleLongName += address[address.length() - 5];
  BleLongName += address[address.length() - 4];
  BleLongName += address[address.length() - 2];
  BleLongName += address[address.length() - 1];
/*
  BLEDevice::init("initial");
  String address = BLEDevice::getAddress().toString().c_str();
  BLEDevice::deinit(true);

  address.toUpperCase();
  BleLongName = "Sci";
  BleLongName += address[address.length() - 5];
  BleLongName += address[address.length() - 4];
  BleLongName += address[address.length() - 2];
  BleLongName += address[address.length() - 1];

*/

  BLEDevice::init(BleLongName.c_str());
  DEBUG_PRINT("BLE Name: ");
  DEBUG_PRINTLN(BleLongName);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *whistlepunkService = pServer->createService(WHISTLEPUNK_SERVICE_UUID);
  valueCharacteristic = whistlepunkService->createCharacteristic("555a0003-0aaa-467a-9538-01f0652c74e8", BLECharacteristic::PROPERTY_NOTIFY);
  BLECharacteristic *configCharacteristic = whistlepunkService->createCharacteristic("555a0010-0aaa-467a-9538-01f0652c74e8", BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic *versionCharacteristic = whistlepunkService->createCharacteristic("555a0011-0aaa-467a-9538-01f0652c74e8", BLECharacteristic::PROPERTY_READ);

  valueCharacteristic->addDescriptor(new BLE2902());
  valueCharacteristic->setCallbacks(new ValueCallbacks());

  configCharacteristic->addDescriptor(new BLE2902());
  configCharacteristic->setCallbacks(new ConfigCallbacks());

  versionCharacteristic->addDescriptor(new BLE2902());
  versionCharacteristic->setCallbacks(new VersionCallbacks());

  // start the service
  whistlepunkService->start();

  int version = (int)goosci_Version_Version_LATEST;
  DEBUG_PRINT("Protocol Version: ");
  DEBUG_PRINTLN(version);
  versionCharacteristic->setValue(version);

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(WHISTLEPUNK_SERVICE_UUID);
  pAdvertising->start();
  DEBUG_PRINT("BLE Advertising.");
}

void loop() {
  unsigned long currentMillis = millis();
  uint32_t sensorValue = 0;

  if (deviceConnected) {
    if (pin_type == P_ANALOG) {
      sensorValue = analogRead(pin);
    } else if (pin_type == P_DIGITAL) {
      sensorValue = digitalRead(pin);
    } else {
      sensorValue = 666;
    }

    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      DEBUG_PRINT("Pin: ");
      DEBUG_PRINT(pin);
      DEBUG_PRINT(" SensorValue: ");
      DEBUG_PRINTLN(sensorValue);
    }
     send_data(valueCharacteristic, millis(), sensorValue);
  } else {
    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      DEBUG_PRINT("."); // just to let us know its alive via serial console
    }
  }
}