#include "mqtt_service.h"

MQTTService::MQTTService(PubSubClient& mqttClient, const char* topic) 
    : _mqttClient(mqttClient), _topic(topic) {
}

MQTTService::~MQTTService() {
}

void MQTTService::sendMessage(const char* topic, const String& message) {
    _mqttClient.publish(topic, message.c_str());
}

void MQTTService::setCallback(std::function<void(MqttCommand)> callback) {
    _callback = callback;
}
