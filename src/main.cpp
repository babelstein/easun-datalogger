#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include "secrets.h"
#include "inverter_commands.h"
#include "inverter_service.h"
#include "mqtt_service.h"
#include "json_serializer.h"

const bool debug = false;

// WiFi client for MQTT
WiFiClient mqttWiFiClient;

// MQTT client
PubSubClient mqttClient(mqttWiFiClient);

// Inverter Service instance
InverterService inverterService(Serial);

// MQTT Service instance
MQTTService mqttService(mqttClient, MQTT_TOPIC);

// Array to store custom commands waiting to be executed
std::vector<MqttCommand> customCommands;

// Callback for MQTT messages
void mqttCallback(MqttCommand command) {
    customCommands.push_back(command);
}

void setup() {
    if(debug) {
        Serial.begin(9600);
        while (!Serial) {
            ; // wait for serial port to connect. Needed for native USB
        }
    } else {
        Serial.begin(2400);
    }

    configTime(0, 0, "pool.ntp.org"); 

    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
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
    if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
    } else {
    }
    
    // Reconnect MQTT loop
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
        }
        delay(5000);
    }
}

// Parse response string - extracts content between parentheses and removes them
String readResult() {
    String commandResult;
    if (Serial.available()) {
        while (Serial.available()) {
            char c = Serial.read();
            
            if (c == '(') {
                continue;
            }
            else if (c == ')') {
                continue;
            }
            else if (c == '\r') {
                return commandResult;
            } 
            else if (c >= 32 && c <= 126) {
                commandResult += c;
            }
            yield();
        }
    }
    Serial.flush();
    return commandResult;
}

void displayResults(String result) {
}

void healthCheck() {
    WiFiClientSecure client;
    HTTPClient http;
    X509List cert(caCert);
    
    client.setTrustAnchors(&cert);
    client.setTimeout(10000);
    http.setTimeout(10000);
    
    http.begin(client, API_URL);
    
    int httpCode = http.GET();
    
    if (httpCode == 200) {
    } else {
    }
    
    http.end();
}

void sendDataArray(String* results) {
    WiFiClientSecure client;
    HTTPClient http;
    X509List cert(caCert);
    
    client.setTrustAnchors(&cert);
    client.setTimeout(10000);
    http.setTimeout(10000);
    
    http.begin(client, API_URL);
    
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", AUTH_TOKEN);
    
    // Budowanie tablicy obiektów JSON
    String jsonObjects = "[";
    for (int i = 0; i < CMD_COUNT; i++) {
        jsonObjects += "{\"operation\": \"" + String(CMD_NAMES[i]) + "\", " +
                       "\"message\": \"" + results[i] + "\"}";
        
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
    // MQTT keep alive loop
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
        }
        delay(5000);
    }

    // Step 1: Execute Inverter Service function that sends all defined commands and return their data as object
    AllCommandResults allResults = inverterService.sendAllCommands();
    String* RawResults = new String[CMD_COUNT];
    RawResults[0] = "\""+allResults.qpigs+"\"";
    RawResults[1] = "\""+allResults.qmod+"\"";
    RawResults[2] = "\""+allResults.qpiri+"\"";
    sendDataArray(RawResults);

    // Step 2: Send each command result to MQTT as separate message via MQTT Service instance
    // Use json_serializer.cpp functions to properly parse structs to json strings
    String* results = new String[CMD_COUNT];
    results[0] = qpigsDataToJson(allResults.qpigsData);
    results[1] = qmodDataToJson(allResults.qmodData);
    results[2] = qpiriDataToJson(allResults.qpiriData);
    
    for (int i = 0; i < CMD_COUNT; i++) {
        String payload = "{\"command\":\"" + String(CMD_NAMES[i]) + "\",\"result\": " + results[i] + "}";
        mqttService.sendMessage(MQTT_TOPIC, payload);
    }
    
    // Step 3: Send each command result to API using sendDataArray() function
    sendDataArray(results);
    
    // Step 4: If there are custom commands waiting to be executed, get first command from array and execute it via Inverter Service function
    if (!customCommands.empty()) {
        MqttCommand customCommand = customCommands[0];
        
        // Step 5: Send executed custom command result via MQTT Service
        CommandResult customResult = inverterService.sendCommand(customCommand.commandName.c_str(), customCommand.parameter);
        mqttService.sendMessage(MQTT_TOPIC, "{\"command\":\"" + customCommand.commandName + "\",\"result\":\"" + customResult.data + "\"}");
        
        // Step 6: Send executed custom command result to API using sendDataArray() function
        String* customResults = new String[1];
        customResults[0] = customResult.data;
        sendDataArray(customResults);
        delete[] customResults;
        
        // Step 7: Remove executed custom command from array
        customCommands.erase(customCommands.begin());
    }
    
    delete[] results;
    delay(1500);
}
