#pragma once
#include <Arduino.h>
#include "../Interface/IDevice.h"
class I2CDevice : public IDevice
{
public:
    virtual ~I2CDevice(){};
    
    virtual bool ReadBytes(uint8_t *buffer, size_t bufferSize) = 0;
    virtual bool WriteBytes(uint8_t *buffer, size_t bufferSize) = 0;

    virtual bool Initialization() = 0;

    virtual uint8_t GetAddress() = 0;

    virtual bool Measure() = 0;
    virtual bool Download() = 0;
    virtual void Calculate() = 0;

    virtual void *BeginTransport();
    virtual bool EndTransport(void *linker, int i2cPort = 0, uint outTime = 1000);
};