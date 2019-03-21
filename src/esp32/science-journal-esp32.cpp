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
#include <BLEUtils.h>
#include "WiFi.h"

#include <pb.h>
#include "goosci_utility.h"
#include "debug_print.h"
#include "config_change.h"
#include "heartbeat.h"
#include "sensor.pb.h"


#define WHISTLEPUNK_UUID "555a0001-0aaa-467a-9538-01f0652c74e8"
//const short int version = goosci_Version_Version_LATEST;
BLEServer* pServer = NULL;
//BLEService* whistlepunkService = NULL;
BLECharacteristic* valueCharacteristic = NULL;
BLECharacteristic* configCharacteristic = NULL;
BLECharacteristic* versionCharacteristic = NULL;

bool deviceConnected = false;
String BleLongName;
extern PinType pin_type;
extern int pin;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      DEBUG_PRINTLN("BLE Connected");
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      DEBUG_PRINTLN("BLE Disconnected");
      deviceConnected = false;
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
      DEBUG_PRINTLN("config change event");
      //std::string v = pCharacteristic->getValue();
      uint8_t* v = pCharacteristic->getData();
      handle(v);
    }
};


void setup() {
  wait_for_serial();

  // I can't seem to initialize the Bluetooth Radio and then change the broadcast
  // name, so we're going to use the WiFi MAC Address instead
  WiFi.mode(WIFI_MODE_STA);
  String address = WiFi.macAddress();
  WiFi.mode(WIFI_OFF);

  address.toUpperCase();
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
  DEBUG_PRINT("Address is: ");
  DEBUG_PRINTLN(BleLongName);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *whistlepunkService = pServer->createService(WHISTLEPUNK_UUID);
  valueCharacteristic = whistlepunkService->createCharacteristic(WHISTLEPUNK_UUID, BLECharacteristic::PROPERTY_NOTIFY);
  configCharacteristic = whistlepunkService->createCharacteristic(WHISTLEPUNK_UUID, BLECharacteristic::PROPERTY_WRITE);
  versionCharacteristic = whistlepunkService->createCharacteristic(WHISTLEPUNK_UUID, BLECharacteristic::PROPERTY_READ);

  //versionCharacteristic->setValue((int)goosci_Version_Version_LATEST);
  int version = (int)goosci_Version_Version_LATEST;
  DEBUG_PRINT("Version: ");
  DEBUG_PRINTLN(version);
  versionCharacteristic->setValue(version);

  // start the service
  whistlepunkService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
  DEBUG_PRINT(F("LongName: "));
  DEBUG_PRINTLN(BleLongName);

  if (deviceConnected) {
    uint32_t sensorValue = 0;
    if (pin_type == P_ANALOG) {
      sensorValue = analogRead(pin);
    } else if (pin_type == P_DIGITAL) {
      sensorValue = digitalRead(pin);
    } else {
      sensorValue = 666;
    }
    DEBUG_PRINT("SensorValue: ");
    DEBUG_PRINTLN(sensorValue);
    send_data(valueCharacteristic, millis(), sensorValue);
  }
#ifdef GOOSCI_DEVELOPER_MODE
  heartbeat();
#endif
  delay(2000);
}