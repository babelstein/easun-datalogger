#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

// Using built-in UART (GPIO1 RX, GPIO3 TX)
// RX = GPIO1, TX = GPIO3
const unsigned long MILISECONDS_DELAY = 1000;
const int MAX_RETRIES = 3;
const bool debug = false;

// Definicje gotowych ramek z obliczonym CRC i znakiem \r
const byte cmd_QPIGS[] = {0x51, 0x50, 0x49, 0x47, 0x53, 0xB7, 0xA9, 0x0D}; 
const byte cmd_QMOD[]  = {0x51, 0x4D, 0x4F, 0x44, 0x49, 0xC1, 0x0D};
const byte cmd_QPI[]   = {0x51, 0x50, 0x49, 0xBE, 0xAC, 0x0D};
const byte cmd_QPIRI[] = {0x51, 0x50, 0x49, 0x52, 0x49, 0xF8, 0x54, 0x0D};

// Tabela wskaźników do komend i ich rozmiarów
const byte* allCommands[] = {cmd_QPIGS, cmd_QMOD, cmd_QPI, cmd_QPIRI};
const int cmdSizes[] = {8, 7, 6, 8};
const char* cmdNames[] = {"QPIGS", "QMOD", "QPI", "QPIRI"};
const int numCommands = 4;

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
  Serial.begin(2400);
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


String readResult(){
  String commandResult;
  if (Serial.available()) {
    while (Serial.available()) {
      char c = Serial.read();
      
      if (c == '\r') {
        return commandResult;
        Serial.flush();
      } 
      // Akceptujemy tylko czytelne znaki ASCII (od spacji do tyldy) oraz nawias (
      else if (c >= 32 && c <= 126) {
        commandResult += c;
      }
      // Bajty CRC (często o wartościach < 32 lub > 126) zostaną tutaj pominięte
      yield(); 
    }
  }
  Serial.flush();
  return commandResult;
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


void sendDataArray(String* results) {
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
  for (int i = 0; i < numCommands; i++) {
    jsonObjects += "{\"operation\": \"" + String(cmdNames[i]) + "\", " +
                   "\"message\": \"" + results[i] + "\"}";
    
    if (i < numCommands - 1) {
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
  
  // ✅ Wymuszenie zwolnienia zasobów (ważne dla ESP8266!)
  http.end();
}


void loop() {
  static String results[numCommands];

  for (int i = 0; i < numCommands; i++) {
    debugPrint("\nWysyłam: " + String(cmdNames[i]));
    
    Serial.write(allCommands[i], cmdSizes[i]);
    results[i] = readResult();
    displayResults(results[i]);
    
    delay(MILISECONDS_DELAY);
  }
  
  sendDataArray(results);
  currentCommandIndex = 0;
  delay(1500);
}

