#include <PubSubClient.h>
#include "secrets.h"
#include "inverter_commands.h"
#include "inverter_service.h"
#include "mqtt_service.h"
#include "json_serializer.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#endif

#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#endif

const int RX2_PIN = 16;
const int TX2_PIN = 17;
const bool debug = false;

// WiFi client for MQTT
WiFiClient mqttWiFiClient;

// MQTT client
PubSubClient mqttClient(mqttWiFiClient);

#ifdef ESP32
    // ESP32: Use UART1 for inverter, UART0 (Serial) for USB-C debugging/upload
    // UART1: GPIO 17 (TX), GPIO 18 (RX) - can be changed in setup()
    InverterService inverterService(Serial2);
#else
    // ESP8266: Use default Serial (only one hardware UART available)
    InverterService inverterService(Serial);
#endif

// MQTT Service instance
MQTTService mqttService(mqttClient, MQTT_TOPIC);

// Array to store custom commands waiting to be executed
std::vector<MqttCommand> customCommands;

// Callback for MQTT messages
void mqttCallback(MqttCommand command) {
    customCommands.push_back(command);
}

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
#ifdef ESP32
    Serial.begin(9600);
    // ESP32: Initialize UART1 for inverter communication
    Serial2.begin(2400, SERIAL_8N1, RX2_PIN, TX2_PIN);
    
#else
    // ESP8266: Serial is used for inverter communication, no separate initialization needed
    if(debug) {
        Serial.begin(115200);
        while (!Serial) {
            ; // wait for serial port to connect. Needed for native USB
        }
    } else {
        Serial.begin(115200);
    }
#endif

    debugPrint("Initializing system...");
    configTime(0, 0, "pool.ntp.org"); 
    
    // Connect to Wi-Fi
    debugPrint("Connecting to WiFi...", false);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int wifiRetryCount = 0;
    int maxWifiRetries = 30;
    while (WiFi.status() != WL_CONNECTED && wifiRetryCount < maxWifiRetries) {
        delay(1000);
        debugPrint(".", false);
        wifiRetryCount++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        debugPrint(" OK");
        debugPrint("WiFi connected, IP: ");
        debugPrint(WiFi.localIP().toString());
    } else {
        debugPrint(" FAILED");
        debugPrint("WiFi connection failed after retries");
    }
    
    // Initialize MQTT
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback([](char* topic, byte* payload, unsigned int length) {
        String message = "";
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        
        // Parse JSON message to extract command name and parameter
        MqttCommand command;
        command.commandName = message.substring(0, message.indexOf(":") + 1);
        command.parameter = message.substring(message.indexOf(":") + 2);
        
        mqttCallback(command);
    });
    mqttClient.setKeepAlive(30);
    mqttClient.setBufferSize(4096);
    
    // Connect to MQTT
    debugPrint("Connecting to MQTT...");
    if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
        debugPrint(" OK");
        debugPrint("MQTT connected, topic: ");
        debugPrint(MQTT_TOPIC);
    } else {
        debugPrint(" FAILED");
        debugPrint("MQTT connect failed, error code: ");
        debugPrint(String(mqttClient.state()));
    }
    
    // Wait for MQTT to be connected (with timeout)
    int mqttRetryCount = 0;
    int maxMqttRetries = 30;
    while (!mqttClient.connected() && mqttRetryCount < maxMqttRetries) {
        if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
            debugPrint("MQTT reconnected successfully");
            break;
        }
        delay(1000);
        mqttRetryCount++;
    }
    if (!mqttClient.connected()) {
        debugPrint("MQTT connection failed after retries");
    }
}

void sendDataToApi(String* results) {
    WiFiClientSecure client;
    HTTPClient http;
    #ifdef ESP8266
        X509List cert(caCert);
        client.setTrustAnchors(&cert);
    #else
        client.setCACert(caCert);
    #endif
    
    client.setTimeout(10000);
    http.setTimeout(10000);
    
    http.begin(client, API_URL);
    
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", AUTH_TOKEN);
    
    // Budowanie tablicy obiektów JSON
    String jsonObjects = "[";
    for (int i = 0; i < CMD_COUNT; i++) {
        jsonObjects += "{\"operation\": \"" + String(CMD_NAMES[i]) + "\", " +
                       "\"message\": " + results[i] + "}";
        
        if (i < CMD_COUNT - 1) {
            jsonObjects += ",";
        }
    }
    jsonObjects += "]";
    
    int httpCode = http.POST(jsonObjects);
    int retryCount = 0;
    
    if (httpCode == 200) {
        retryCount = 0;
    } else {
        if (httpCode == 401 || httpCode == 400) {
            retryCount = 0;
        } else if (retryCount < MAX_RETRIES) {
            retryCount++;
            
            // Opóźnienie przed ponowną próbą
            unsigned long waitTime = 1000 * (1 << (retryCount - 1)); // 1s, 2s, 4s
            
            // Ponowna próba
            httpCode = http.POST(jsonObjects);
            
            if (httpCode == 200) {
                retryCount = 0;
            }
        }
    }
    
    String serverResponse = http.getString();
    
    http.end();
}

void loop() {
    // WiFi connection check and reconnection
    if (WiFi.status() != WL_CONNECTED) {
        debugPrint("WiFi disconnected, attempting to reconnect...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        int retryCount = 0;
        while (WiFi.status() != WL_CONNECTED && retryCount < 10) {
            delay(1000);
            retryCount++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            debugPrint("WiFi reconnected successfully");
        } else {
            debugPrint("WiFi reconnection failed");
        }
        // Wait before continuing operations if WiFi is still not connected
        if (WiFi.status() != WL_CONNECTED) {
            delay(5000);
        }
    }

    // MQTT keep alive loop (only if WiFi is connected)
    if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
        debugPrint("MQTT disconnected, attempting to reconnect...");
        if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
            debugPrint("MQTT reconnected successfully");
        } else {
            debugPrint("MQTT reconnection failed");
        }
    }

    // Step 1: Execute Inverter Service function that sends all defined commands and return their data as object
    AllCommandResults allResults;
    allResults = inverterService.sendAllCommands();
    allResults = inverterService.parseAllCommands(allResults);
    
    // Debug: Print parsed results
    debugPrint("=== Parsed Command Results ===");
    debugPrint("QPIGS: " + allResults.qpigs);
    debugPrint("QMOD: " + allResults.qmod);
    debugPrint("QPIRI: " + allResults.qpiri);

    // Step 2: Send each command result to MQTT as separate message via MQTT Service instance
    // Use json_serializer.cpp functions to properly parse structs to json strings
    String* results = new String[CMD_COUNT];
    results[0] = qpigsDataToJson(allResults.qpigsData);
    results[1] = qmodDataToJson(allResults.qmodData);
    results[2] = qpiriDataToJson(allResults.qpiriData);
    
    // Debug: Print JSON results
    debugPrint("=== JSON Results ===");
    debugPrint("QPIGS JSON: " + results[0]);
    debugPrint("QMOD JSON: " + results[1]);
    debugPrint("QPIRI JSON: " + results[2]);
    
    for (int i = 0; i < CMD_COUNT; i++) {
        String payload = "{\"command\":\"" + String(CMD_NAMES[i]) + "\",\"result\": " + results[i] + "}";
        mqttService.sendMessage(MQTT_TOPIC, payload);
    }
    
    // Step 3: Send each command result to API using sendDataToApi() function
    sendDataToApi(results);
    
    // Step 4: If there are custom commands waiting to be executed, get first command from array and execute it via Inverter Service function
    if (!customCommands.empty()) {
        MqttCommand customCommand = customCommands[0];
        
        // Step 5: Send executed custom command result via MQTT Service
        CommandResult customResult = inverterService.sendCommand(customCommand.commandName.c_str(), customCommand.parameter);
        mqttService.sendMessage(MQTT_TOPIC, "{\"command\":\"" + customCommand.commandName + "\",\"result\":\"" + customResult.data + "\"}");
        
        // Step 6: Send executed custom command result to API using sendDataArray() function
        String* customResults = new String[1];
        customResults[0] = customResult.data;
        sendDataToApi(customResults);
        delete[] customResults;
        
        // Step 7: Remove executed custom command from array
        customCommands.erase(customCommands.begin());
    }
    
    delete[] results;
    // delay(1500);
}
