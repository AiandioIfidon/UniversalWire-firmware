#pragma once


const char* SSID = "An_D";
const char* PASSWORD = "2763fundy";


void connectToWifi(String ssid, String password) {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected, IP: ");
  Serial.println(WiFi.localIP());
}

bool checkTransaction(String digest) {
  // connectToWifi(SSID, PASSWORD);
  bool success = false;
  if( digest.length() % 2 == 0){
    success = true;
  }
  Serial.print("This is the digest status: ");
  Serial.println(success);
  return success;
}


void dispense() {

}