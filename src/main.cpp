#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ModbusMaster.h>
#include "secrets.h"
#include "modbus_registers.h"

// Modbus configuration
#define MODBUS_SLAVE_ID 1              // SMG-II slave ID
#define MODBUS_BAUD_RATE 9600         // Baud rate for Modbus RTU
#define TX_PIN 4                       // GPIO4 (D2 on NodeMCU)
#define RX_PIN 5                       // GPIO5 (D1 on NodeMCU)

// Create ModbusMaster object
ModbusMaster modbusNode;

// Retry configuration
const unsigned long MILISECONDS_DELAY = 100;
const int MAX_RETRIES = 3;
const bool debug = false;

int retryCount = 0;
int currentCommandIndex = 0;

void debugPrint(String message, bool newLine = true) {
  if (debug) {
    if (newLine) {
      Serial.println(message);
    } else {
      Serial.print(message);
    }
  }
}

void setup() {
  configTime(0, 0, "pool.ntp.org"); 
  
  //hardware uart and modbus communiciation setup
  Serial.begin(9600);
  modbusNode.begin(MODBUS_SLAVE_ID, Serial);
  debugPrint("Rozpoczynam podsłuchiwanie sekwencyjne. . .");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debugPrint(".", false);
  }
  debugPrint("");
  debugPrint("Wi-Fi connected");
}

void displayResults(String result){
  debugPrint("Otrzymany wynik: " + result);
}

void healthCheck(){
  debugPrint("Wysyłanie żądania health check do serwera. . .");

  WiFiClientSecure client;
  HTTPClient http;
  X509List cert(caCert);
  
  client.setTrustAnchors(&cert);
  client.setTimeout(10000);
  http.setTimeout(10000);
  
  http.begin(client, api_url);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    debugPrint("✅ Health check successful with code: " + String(httpCode));
    String serverResponse = http.getString();
    debugPrint("Server response: " + serverResponse);
  } else {
    debugPrint("❌ Health check failed with code: " + String(httpCode));
  }
  
  // ✅ Wymuszenie zwolnienia zasobów (ważne dla ESP8266!)
  http.end();
}


void sendDataArray(uint8_t* results) {
  debugPrint("Wysyłanie danych do inwertera. . .");
  
  WiFiClientSecure client;
  HTTPClient http;
  X509List cert(caCert);
  
  client.setTrustAnchors(&cert);
  client.setTimeout(10000);
  http.setTimeout(10000);
  
  http.begin(client, api_url);
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", auth_token);
  
  // Budowanie tablicy obiektów JSON
  String jsonObjects = "[";
  for (int i = 0; i < numReadRegisters; i++) {
    jsonObjects += "{\"operation\": \"" + String(readRegisters[i].name) + "\", " +
                   "\"message\": \"" + String(results[i]) + "\"}";
    
    if (i < numReadRegisters - 1) {
      jsonObjects += ",";
    }
  }
  jsonObjects += "]";
  
  debugPrint("Wysyłam payload: " + jsonObjects);
  
  int httpCode = http.POST(jsonObjects);
  
  if (httpCode == 200) {
    debugPrint("✅ POST request successful with code: " + String(httpCode));
    retryCount = 0;
  } else {
    if (httpCode == 401 || httpCode == 400) {
      debugPrint("❌ POST request failed with code: " + String(httpCode));
      retryCount = 0;
    } else if (retryCount < MAX_RETRIES) {
      retryCount++;
      debugPrint("❌ Retry attempt " + String(retryCount) + " of " + String(MAX_RETRIES));
      
      // ✅ Opóźnienie w milisekundach (Arduino delay)
      unsigned long waitTime = 1000 * (1 << (retryCount - 1)); // 1s, 2s, 4s
      debugPrint("Wait " + String(waitTime) + " ms before retrying...");
      
      // ✅ Opóźnienie przed ponowną próbą
      delay(waitTime);
      
      // ✅ Ponowna próba
      httpCode = http.POST(jsonObjects);
      
      if (httpCode == 200) {
        debugPrint("✅ Retry successful with code: " + String(httpCode));
        retryCount = 0;
      } else {
        debugPrint("❌ Retry failed with code: " + String(httpCode));
      }
    } else {
      debugPrint("❌ Max retries reached. Giving up.");
      retryCount = 0;
    }
  }
  
  String serverResponse = http.getString();
  debugPrint("Server response: " + serverResponse);
  
  http.end();
}


void loop() {
  uint8_t result;
  static uint8_t results[numReadRegisters];

  for (int i = 0; i < numReadRegisters; i++) {
    debugPrint("\nWysyłam: " + String(readRegisters[i].name));
    
    // Użyj długości rejestru z tablicy readRegisters
    result = modbusNode.readHoldingRegisters(readRegisters[i].address, readRegisters[i].length);
    if (result == modbusNode.ku8MBSuccess) {
      results[i] = modbusNode.getResponseBuffer(0) * readRegisters[i].scale;
      displayResults(String(results[i]) + " " + readRegisters[i].unit);
    }
    
    delay(MILISECONDS_DELAY);
  }
  
  sendDataArray(results);
  delay(1500);
}

