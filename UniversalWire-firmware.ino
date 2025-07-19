#define UNIWIRE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

int counter = 0;
const uint8_t MAX_CREDENTIAL_LENGTH = 32;
const String DEVICE_NAME = "Transceiver";  // ✅ Use String, not string
String g_received = "";  // ✅ Correct type
String g_sent = "";

const String SERVICE_UUID = "853f29b2-f5ed-4b69-b4c6-9cd68a9fc2b0";
const String SEND_UUID = "b72b9432-25f9-4c7f-96cb-fcb8efde84fd";

class TransceiverCallback : public BLECharacteristicCallbacks {
private:
  const size_t maxLength;

public:
  TransceiverCallback(size_t maxLen)
    : maxLength(maxLen) {}

  void onWrite(BLECharacteristic *pCharacteristic) override {
    String value = pCharacteristic->getValue();
    g_received = String(value.c_str()); 
    if (g_received.length() >= maxLength) {
      Serial.println(F("Error: Received value too long"));
      return;
    }

    Serial.print("Received: ");
    Serial.println(g_received);

    pCharacteristic->setValue(g_sent.c_str());
    pCharacteristic->notify();

    Serial.print("Sent: ");
    Serial.println(g_sent);
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
  delay(500);
  counter++;
  g_sent = String(counter);
  Serial.print("Received: ");
  Serial.println(g_received);
}
