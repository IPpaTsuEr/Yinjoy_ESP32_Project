#pragma once
#include "IJson.h"
enum DviceProtocolType
{
    I2C,
    SPI,
    Other
};
enum DeviceType
{
    Sensor,
    Switcher,
    Storage,
    Display
};

class IDevice : public IJson
{

public:
    virtual DviceProtocolType GetDeviceProtocolType() = 0;
    virtual DeviceType GetDeviceType() = 0;
    virtual uint8_t GetDeviceID() = 0;
    virtual String GetDeviceName() = 0;
};