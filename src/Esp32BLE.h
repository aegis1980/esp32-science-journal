
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

class Esp32BLE {

    
    class MyServerCallbacks: public BLEServerCallbacks {

        void onConnect(BLEServer* pServer) {
            DEBUG_PRINTLN("BLE Client Connected");
            parent.connected = true;
        };

        void onDisconnect(BLEServer* pServer) {
            DEBUG_PRINTLN("BLE Client Disconnected");
            parent.connected = false;
        }

        // To access parent member variable 'connected' in this inner class.
        public: 
            MyServerCallbacks(Esp32BLE& x): parent(x) {}

        private:
            Esp32BLE&  parent;
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

        handle(v, sizeof(v)/sizeof(uint8_t));
        }

        void onRead(BLECharacteristic *pCharacteristic) {
        DEBUG_PRINTLN("configCharacteristic onRead()");
        }
    };


    private:
        BLECharacteristic* valueCharacteristic = NULL;
        String BleLongName;

    public:
        bool connected = false;
        void init() {
            wait_for_serial();
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("");
            DEBUG_PRINTLN("Esp32Ble Startup");
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
            pServer->setCallbacks(new MyServerCallbacks(*this));

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

        void sendValue(uint32_t val){
            send_data(valueCharacteristic, millis(), val);
        }

};