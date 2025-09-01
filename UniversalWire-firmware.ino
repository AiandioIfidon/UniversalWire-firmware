#define UNIWIRE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "transactionChecker.hpp"

int counter = 0;

const uint8_t MAX_CREDENTIAL_LENGTH = 32;
const String DEVICE_NAME = "Transceiver";
String g_digest = ""; 
String g_sent = "not checked";

bool isChecked = false;
bool isChecking = false;
bool success = false;


const String SERVICE_UUID = "853f29b2-f5ed-4b69-b4c6-9cd68a9fc2b0"; // these UUIDs were generated using an online UUID generator.
const String SEND_UUID = "b72b9432-25f9-4c7f-96cb-fcb8efde84fd"; // they can be anything but they have to match with the app and this is what I used in the app.

class TransceiverCallback : public BLECharacteristicCallbacks {
private:
  const size_t maxLength;

public:
  TransceiverCallback(size_t maxLen)
    : maxLength(maxLen) {}

  // this is the function that is executed when the device receives a value
  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue(); // received value
    g_digest = value;
    Serial.print("Digest Receive: ");
    Serial.println(g_digest);

    if(isChecking) {
      g_sent = "Is being checked";
    }
    if(isChecked) {
      g_sent = success? "success" : "failed";
    }
    pCharacteristic->setValue(g_sent.c_str()); // send a value to the mobile app
    pCharacteristic->notify();
    Serial.print("Sent: ");
    Serial.println(g_sent);
    
    
    if(!isChecking) {
      isChecking = true;
      success = checkTransaction(g_digest);
      isChecked = true;
    }
  }
};

void setupBLEServer() {
  BLEDevice::init(DEVICE_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  if (!pServer) {
    Serial.println(F("Failed to create BLE server"));
    return;
  }

  BLEService *pService = pServer->createService(SERVICE_UUID);
  if (!pService) {
    Serial.println(F("Failed to create BLE service"));
    return;
  }

  BLECharacteristic *Characteristic = pService->createCharacteristic(
    SEND_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  if (!Characteristic) {
    Serial.println(F("Failed to create characteristics"));
    return;
  }

  Characteristic->setCallbacks(new TransceiverCallback(MAX_CREDENTIAL_LENGTH));

  pService->start();
 
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // iPhone connection help
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println(F("BLE server started successfully"));
}

void setup() {
  Serial.begin(115200);
  setupBLEServer();
}

void loop() {
  delay(1500);
}
