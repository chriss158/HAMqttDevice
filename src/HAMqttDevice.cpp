#include "HAMqttDevice.h"

HAMqttDevice::HAMqttDevice(
    const String &name,
    const DeviceType type,
    const String &haMQTTPrefix,
    const String &nodeId) : _name(name),
                            _type(type)
{
    // Id = name to lower case replacing spaces by underscore (ex: name="Kitchen Light" -> id="kitchen_light")
    _identifier = name;
    _identifier.replace(' ', '_');
    _identifier.toLowerCase();
    _nodeId = nodeId;
    _mqttPrefix = haMQTTPrefix;

    // Define the MQTT topic of the device
    _topic = generateTopic();

    // Preconfigure mandatory config vars that we already know
    addConfigVar("~", _topic);
    addConfigVar("name", _name);
    addConfigVar("unique_id", _identifier);

    // When the command topic is mandatory, enable it.
    switch (type)
    {
    case DeviceType::ALARM_CONTROL_PANEL:
    case DeviceType::COVER:
    case DeviceType::FAN:
    case DeviceType::LIGHT:
    case DeviceType::LOCK:
    case DeviceType::NUMBER:
    case DeviceType::SWITCH:
    case DeviceType::BUTTON:
        enableCommandTopic();
    default:
        break;
    }

    // When the state topic is mandatory, enable it.
    switch (type)
    {
    case DeviceType::ALARM_CONTROL_PANEL:
    case DeviceType::BINARY_SENSOR:
    case DeviceType::COVER:
    case DeviceType::FAN:
    case DeviceType::LIGHT:
    case DeviceType::LOCK:
    case DeviceType::NUMBER:
    case DeviceType::SENSOR:
    case DeviceType::SWITCH:
        enableStateTopic();
    default:
        break;
    }
}

HAMqttDevice::~HAMqttDevice() {}

HAMqttDevice &HAMqttDevice::enableCommandTopic()
{
    addConfigVar("cmd_t", "~/cmd");
    return *this;
}

HAMqttDevice &HAMqttDevice::enableStateTopic()
{
    addConfigVar("stat_t", "~/state");
    return *this;
}

HAMqttDevice &HAMqttDevice::enableAttributesTopic()
{
    addConfigVar("json_attr_t", "~/attr");
    return *this;
}

HAMqttDevice &HAMqttDevice::enableAvailabilityTopic(const String &topic)
{
    addConfigVar("avty_t", (topic == NULL || topic.isEmpty()) ? "~/availability" : topic);
    return *this;
}

HAMqttDevice &HAMqttDevice::addConfigVar(const String &name, const String &value)
{
    _configVars.push_back({name, value});
    return *this;
}
HAMqttDevice &HAMqttDevice::remConfigVar(const String &key)
{
    _configVars.erase(
        std::remove_if(_configVars.begin(), _configVars.end(), [&](ConfigVar const &configVar)
                       { return configVar.key == key; }),
        _configVars.end());
    return *this;
}

HAMqttDevice &HAMqttDevice::addAttribute(const String &name, const String &value)
{
    _attributes.push_back({name, value});
    return *this;
}

HAMqttDevice &HAMqttDevice::setNodeId(const String &nodeId)
{
    _nodeId = nodeId;
    _topic = generateTopic();
    remConfigVar("~");
    //_configVars.insert(_configVars.begin(), {"~", _topic});
    addConfigVar("~", _topic);
    return *this;
}

HAMqttDevice &HAMqttDevice::clearAttributes()
{
    _attributes.clear();
    return *this;
}

const String HAMqttDevice::getConfigPayload() const
{
    String configPayload = "{";

    for (uint8_t i = 0; i < _configVars.size(); i++)
    {
        configPayload.concat('"');
        configPayload.concat(_configVars[i].key);
        configPayload.concat("\":");

        bool valueIsDictionnary = _configVars[i].value[0] == '{' && _configVars[i].value[1] != '{';

        if (!valueIsDictionnary)
            configPayload.concat('"');

        configPayload.concat(_configVars[i].value);

        if (!valueIsDictionnary)
            configPayload.concat('"');

        configPayload.concat(',');
    }
    configPayload.setCharAt(configPayload.length() - 1, '}');

    return configPayload;
}

const String HAMqttDevice::getAttributesPayload() const
{
    String attrPayload = "{";

    for (uint8_t i = 0; i < _attributes.size(); i++)
    {
        attrPayload.concat('"');
        attrPayload.concat(_attributes[i].key);
        attrPayload.concat("\":\"");
        attrPayload.concat(_attributes[i].value);
        attrPayload.concat("\",");
    }
    attrPayload.setCharAt(attrPayload.length() - 1, '}');

    return attrPayload;
}

String HAMqttDevice::generateTopic()
{
    if (_nodeId == NULL || _nodeId.isEmpty())
    {
        return _mqttPrefix + '/' + deviceTypeToStr(_type) + '/' + _identifier;
    }
    else
    {
        return _mqttPrefix + '/' + deviceTypeToStr(_type) + '/' + _nodeId + '/' + _identifier;
    }
}

String HAMqttDevice::deviceTypeToStr(DeviceType type)
{
    switch (type)
    {
    case DeviceType::ALARM_CONTROL_PANEL:
        return "alarm_control_panel";
    case DeviceType::BINARY_SENSOR:
        return "binary_sensor";
    case DeviceType::CAMERA:
        return "camera";
    case DeviceType::COVER:
        return "cover";
    case DeviceType::FAN:
        return "fan";
    case DeviceType::LIGHT:
        return "light";
    case DeviceType::LOCK:
        return "lock";
    case DeviceType::SENSOR:
        return "sensor";
    case DeviceType::SWITCH:
        return "switch";
    case DeviceType::CLIMATE:
        return "climate";
    case DeviceType::VACUUM:
        return "vacuum";
    case DeviceType::NUMBER:
        return "number";
    case DeviceType::BUTTON:
        return "button";
    default:
        return "[Unknown DeviceType]";
    }
}
