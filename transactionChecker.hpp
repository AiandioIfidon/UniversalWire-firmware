#include "WiFiType.h"
#pragma once

#include <HTTPClient.h>
#include <WiFi.h>

const char* SSID = "An_D";
const char* PASSWORD = "2763fundy";
const char* suiRpcUrl = "https://fullnode.testnet.sui.io:443";


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



bool checkTransaction(String digest){
  if(WiFi.status() != WL_CONNECTED) {
    connectToWifi(SSID, PASSWORD);
  }
  if(WiFi.status() != WL_CONNECTED){
    return false;
  }
  else{
    HTTPClient http;
    http.begin(suiRpcUrl);
    http.addHeader("Content-Type", "application/json" );

    String payload = "{\"jsonrpc\":\"2.0\",\"id\":1,\"method\":\"sui_getTransactionBlock\",\"params\":[\"" + digest + "\", {\"showInput\": true, \"showRawInput\": false, \"showEffects\": true, \"showEvents\": true, \"showObjectChanges\": false, \"showBalanceChanges\": false, \"showRawEffects\": false}]}";
    int responseCode = http.POST(payload);
    Serial.println(responseCode);

    //change 415 to 200
    if(responseCode == 200){
      String response = http.getString();
      Serial.println(response);

      if(response.indexOf("\"status\":\"success\"") != -1){
        return true;
      }
      else{
        return false;
      }
    }
    else{
      return false;
    }
    http.end();
  }
}