#ifdef UNIWIRE


Preferences preferences;

void store(String inputValue, const char* name){
  preferences.begin(PREF_NAMESPACE, false);
  preferences.putString(name, inputValue);
  preferences.end();
  Serial.print("Updated ");
  Serial.println(name);
  lcdclear();
  lcd_print_offline("Updated ", name);
}

class TransceiverCallback: public BLECharacteristicCallbacks {
private:
    String storage;
    const char* name;
    const size_t maxLength;

public:
    TransceiverCallback(String storage, const char* name, size_t maxLen) 
        : storage(storage), name(name), maxLength(maxLen) {}

    void onWrite(BLECharacteristic *pCharacteristic) override {
        String value = pCharacteristic->getValue();
        if (value.length() >= maxLength) {
            Serial.println(F("Error: Received value too long"));
            return;
        }
        
        storage = value;
        
        Serial.print(name);
        Serial.print(F(" received: "));
        Serial.println(storage);
        store(storage, name);
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

    BLECharacteristic *sendCharacteristic = pService->createCharacteristic(
        SEND_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    BLECharacteristic *recieveCharacteristic = pService->createCharacteristic(
        RECEIVE_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    if (!sendCharacteristic || !recieveCharacteristic) {
        Serial.println(F("Failed to create characteristics"));
        return;
    }

    sendCharacteristic->setCallbacks(new TransceiverCallback(g_sent, "send", MAX_CREDENTIAL_LENGTH));
    recieveCharacteristic->setCallbacks(new TransceiverCallback(g_received, "received", MAX_CREDENTIAL_LENGTH));

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // iPhone connection help
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println(F("BLE server started successfully"));
    return;
}
 void getWiFiCredentials() {
    if (!preferences.begin(PREF_NAMESPACE, true)) {
        Serial.println(F("Failed to open preferences"));
        return;
    }
    
    g_sent = preferences.getString("ssid", "");
    g_received = preferences.getString("password", "");
    
    if (g_sent.length() >= MAX_CREDENTIAL_LENGTH || 
        g_received.length() >= MAX_CREDENTIAL_LENGTH) {
        Serial.println(F("Stored credentials too long, changing is to less that 64 characters is adviced"));
        preferences.end();
        return;
    }
    Serial.println("Found wifi credentials");
    Serial.print("ssid: ");
    Serial.println(g_sent);
    Serial.print("password: ");
    Serial.println(g_received);
    preferences.end();
    return;
}
void Credentials_Change() {
  setupBLEServer();
  updatingCredentials = true;
  g_received = "";
  g_sent = "";
  store(g_sent, "ssid");
  store(g_received, "password");

  while(g_sent == "" && g_received == ""){
    getWiFiCredentials();
    Serial.println("\nWaiting for changes");
    lcdclear();
    lcd_print_offline("Waiting for ", "credentials");
    delay(1000);
  }
  store(g_sent, "ssid");
  store(g_received, "password");
  getWiFiCredentials();
  updatingCredentials = false;
  Serial.println("Successfully changed wifi credentials");
  lcdclear();
  lcd_print_offline("Successfully cha", "nged credentials");
  delay(2000);
}
void setuphandling(){
  // Open preferences in read-only mode
  preferences.begin(PREF_NAMESPACE, true);
  bool hasCredentials = preferences.isKey("ssid");
  preferences.end();

  if (!hasCredentials) {
    Serial.println(F("No stored credentials, starting BLE..."));
    lcdclear();
    lcd_print_offline("No stored creden", "ials, BLE start");
    Credentials_Change();
  } else {
    getWiFiCredentials();
    if (g_sent[0] == '\0' && g_received[0] == '\0') {
      Credentials_Change();
    }
  }
}


#endif