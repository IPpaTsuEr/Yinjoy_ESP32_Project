#pragma once
#include <driver/i2c.h>
#include "../Interface/IJson.h"
#include "I2CDevice.h"

class AHT21B : public I2CDevice
{
private:
    uint8_t DHT21B_Address = 0x38;
    uint8_t DHT21B_Mesure = 0xAC;
    uint8_t DHT21B_MesureParematerOne = 0x33;

    uint8_t DHT21B_Reset = 0xBA;
    uint8_t DHT21B_NorMode = 0xA8;
    uint8_t DHT21B_Init = 0xBE;
    uint8_t DHT21B_InitParemeterOne = 0x08;

    uint8_t DHT21B_EndParematerTwo = 0x00;
    uint8_t DHT21B_IsBusy = 0x80;
    uint8_t DHT21B_IsInited = 0x8;

    uint8_t DHT21B_MesureWaitTime = 80;
    uint8_t DHT21B_Read = 0x1;
    uint8_t DHT21B_Write = 0x0;

    String _name;
    uint8_t _address = DHT21B_Address;
    float_t _temp, _shidu;
    int _i2cPort;
    uint32_t _outTime = 1000;

    bool NeedInitDevice = false;
    uint8_t MesureCmdData[3] = {DHT21B_Mesure, DHT21B_MesureParematerOne, DHT21B_EndParematerTwo};
    uint8_t InitCmdData[3] = {DHT21B_Init, DHT21B_InitParemeterOne, DHT21B_EndParematerTwo};
    uint8_t MesureResultData[5] = {0};

public:
    AHT21B(String name, int I2CPort) : _name(name), _i2cPort(I2CPort){};
    ~AHT21B(){};

    bool ReadBytes(uint8_t *buffer, size_t bufferSize) override;
    bool WriteBytes(uint8_t *buffer, size_t bufferSize) override;

    bool Initialization() override;
    uint8_t GetAddress() override;

    String GetJsonString() override;

    bool Measure()
    {
        return WriteBytes(MesureCmdData, 3);
    };
    bool Download() override
    {
        return ReadBytes(MesureResultData, 5);
    };
    void Calculate() override;

    DviceProtocolType GetDeviceProtocolType() override
    {
        return DviceProtocolType::I2C;
    };
    DeviceType GetDeviceType() override
    {
        return DeviceType::Sensor;
    };
    uint8_t GetDeviceID() override
    {
        return DHT21B_Address;
    };
    String GetDeviceName() override
    {
        return _name;
    };
};
