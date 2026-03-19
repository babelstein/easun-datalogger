#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "secrets.h"

// RX = D1 (GPIO5), TX = D2 (GPIO4)
SoftwareSerial loggerSniffer(D1, D2); 
const unsigned long MILISECONDS_DELAY = 10;

// ✅ Zmienne do obsługi retry
const int MAX_RETRIES = 3;
unsigned long lastRetryTime = 0;
int retryCount = 0;

// Definicje gotowych ramek z obliczonym CRC i znakiem \r
byte cmd_QPIGS[] = {0x51, 0x50, 0x49, 0x47, 0x53, 0xB7, 0xA9, 0x0D}; 
byte cmd_QMOD[]  = {0x51, 0x4D, 0x4F, 0x44, 0x49, 0xC1, 0x0D};
byte cmd_QPI[]   = {0x51, 0x50, 0x49, 0xBE, 0xAC, 0x0D};
byte cmd_QPIRI[] = {0x51, 0x50, 0x49, 0x52, 0x49, 0xF8, 0x54, 0x0D};
byte cmd_QVIRI[] = {0x51, 0x56, 0x49, 0x52, 0x49, 0x1B, 0x34, 0x0D};

// Tabela wskaźników do komend i ich rozmiarów
byte* allCommands[] = {cmd_QPIGS, cmd_QMOD, cmd_QPI, cmd_QPIRI, cmd_QVIRI};
int cmdSizes[] = {8, 7, 6, 8, 8};
const char* cmdNames[] = {"QPIGS", "QMOD", "QPI", "QPIRI", "QVIRI"};

int currentCommandIndex = 0;
const int numCommands = 5;

void setup() {
  configTime(0, 0, "pool.ntp.org"); 
  Serial.begin(9600);
  loggerSniffer.begin(2400); 
  Serial.println("Rozpoczynam podsłuchiwanie sekwencyjne. . .");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
}


String readResult(){
  String commandResult;
  if (loggerSniffer.available()) {
    while (loggerSniffer.available()) {
      char c = loggerSniffer.read();
      
      if (c == '\r') {
        return commandResult;
      } 
      // Akceptujemy tylko czytelne znaki ASCII (od spacji do tyldy) oraz nawias (
      else if (c >= 32 && c <= 126) {
        commandResult += c;
      }
      // Bajty CRC (często o wartościach < 32 lub > 126) zostaną tutaj pominięte
      yield(); 
    }
  }
  return commandResult;
}

void displayResults(String result){
  Serial.println("Otrzymany wynik: " + result);
}

void healthCheck(){
  Serial.println("Wysyłanie żądania health check do serwera. . .");

  WiFiClientSecure client;
  HTTPClient http;
  X509List cert(caCert);
  
  client.setTrustAnchors(&cert);
  client.setTimeout(10000);
  http.setTimeout(10000);
  
  http.begin(client, api_url);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    Serial.println("✅ Health check successful with code: " + String(httpCode));
    
    String serverResponse = http.getString();
    Serial.println("Server response: " + serverResponse);
  } else {
    Serial.print("❌ Health check failed with code: ");
    Serial.println(httpCode);
  }
  
  // ✅ Wymuszenie zwolnienia zasobów (ważne dla ESP8266!)
  http.end();
}


void sendData(String message) {
  Serial.println("Wysyłanie danych do inwertera. . .");
  
  WiFiClientSecure client;
  HTTPClient http;
  X509List cert(caCert);
  
  client.setTrustAnchors(&cert);
  client.setTimeout(10000);
  http.setTimeout(10000);
  
  http.begin(client, api_url);
  
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", auth_token);
  
  String jsonPayload = "{\"operation\": \"" + String(cmdNames[currentCommandIndex]) + "\", " +
                       "\"message\": \"" + message + "\"}";
  
  Serial.println("Wysyłam payload: " + jsonPayload);
  
  int httpCode = http.POST(jsonPayload);
  
  if (httpCode == 200) {
    Serial.println("✅ POST request successful with code: " + String(httpCode));
    retryCount = 0;
  } else {
    if (httpCode == 401 || httpCode == 400) {
      Serial.print("❌ POST request failed with code: ");
      Serial.println(httpCode);
      retryCount = 0;
    } else if (retryCount < MAX_RETRIES) {
      retryCount++;
      Serial.print("❌ Retry attempt ");
      Serial.print(retryCount);
      Serial.print(" of ");
      Serial.println(MAX_RETRIES);
      
      // ✅ Opóźnienie w milisekundach (Arduino delay)
      unsigned long waitTime = 1000 * (1 << (retryCount - 1)); // 1s, 2s, 4s
      Serial.print("Wait ");
      Serial.print(waitTime);
      Serial.println("ms before retry...");
      
      // ✅ Użycie Arduino delay()
      delay(waitTime);
      
      // ✅ Ponowna próba
      httpCode = http.POST(jsonPayload);
      
      if (httpCode == 200) {
        Serial.println("✅ Retry successful with code: " + String(httpCode));
        retryCount = 0;
      } else {
        Serial.print("❌ Retry failed with code: ");
        Serial.println(httpCode);
      }
    } else {
      Serial.println("❌ Max retries reached. Giving up.");
      retryCount = 0;
    }
  }
  
  String serverResponse = http.getString();
  Serial.println("Server response: " + serverResponse);
  
  // ✅ Wymuszenie zwolnienia zasobów (ważne dla ESP8266!)
  http.end();
}


void loop() {
  static unsigned long lastTime = 0;
  
  if (millis() - lastTime > MILISECONDS_DELAY) {
    lastTime = millis();
    
    Serial.print("\nWysyłam: ");
    Serial.println(cmdNames[currentCommandIndex]);
    
    loggerSniffer.write(allCommands[currentCommandIndex], cmdSizes[currentCommandIndex]);

    String result = readResult();
    displayResults(result);
    // healthCheck();
    sendData(result);
    
    currentCommandIndex = (currentCommandIndex + 1) % numCommands;
  } 
}

