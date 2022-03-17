#ifndef HA_MQTT_DEVICE_H
#define HA_MQTT_DEVICE_H

#include "Arduino.h"

class HAMqttDevice
{
public:
    enum DeviceType
    {
        ALARM_CONTROL_PANEL,
        BINARY_SENSOR,
        CAMERA,
        COVER,
        FAN,
        LIGHT,
        LOCK,
        SENSOR,
        SWITCH,
        CLIMATE,
        VACUUM,
        NUMBER,
        BUTTON
    };

private:
    // Device proprieties
    const String _name;
    const DeviceType _type;
    

    String _identifier;
    String _nodeId;
    String _mqttPrefix;
    String _topic;

    // Config variables handling
    struct ConfigVar
    {
        String key;
        String value;
    };
    std::vector<ConfigVar> _configVars;

    // Device attributes handling
    struct Attribute
    {
        String key;
        String value;
    };
    std::vector<Attribute> _attributes;

public:
    HAMqttDevice(
        const String &name,
        const DeviceType type,
        const String &haMQTTPrefix = "ha",
        const String &nodeId = "");

    ~HAMqttDevice();

    HAMqttDevice &enableCommandTopic();
    HAMqttDevice &enableStateTopic();
    HAMqttDevice &enableAttributesTopic();
    HAMqttDevice &enableAvailabilityTopic(const String &topic = "");

    HAMqttDevice &addConfigVar(const String &key, const String &value);
    HAMqttDevice &remConfigVar(const String &key);
    HAMqttDevice &addAttribute(const String &key, const String &value);
    HAMqttDevice &clearAttributes();
    HAMqttDevice &setNodeId(const String &nodeId);

    const String getConfigPayload() const;
    const String getAttributesPayload() const;

    

    inline const String getTopic() const { return _topic; }
    inline const String getAvailabilityTopic() const { return _topic + "/availability"; }
    inline const String getStateTopic() const { return _topic + "/state"; }
    inline const String getConfigTopic() const { return _topic + "/config"; }
    inline const String getAttributesTopic() const { return _topic + "/attr"; }
    inline const String getCommandTopic() const { return _topic + "/cmd"; }

private:
    static String deviceTypeToStr(DeviceType type);
    String generateTopic();

};

#endif