#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include "PubSubClient.h"
#include "inverter_service.h"

// Structure to hold command from MQTT topic
struct MqttCommand {
    String commandName;
    String parameter;
};

class MQTTService {
public:
    MQTTService(PubSubClient& mqttClient, const char* topic);
    ~MQTTService();

    // Send message to MQTT topic
    void sendMessage(const char* topic, const String& message);

    // Read messages from MQTT topic (callback function)
    // Returns object with command name and parameter
    void setCallback(std::function<void(MqttCommand)> callback);

private:
    PubSubClient& _mqttClient;
    const char* _topic;
    std::function<void(MqttCommand)> _callback;
};

#endif // MQTT_SERVICE_H
