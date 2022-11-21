#include "Header/I2CDevice/I2CDevice.h"
#include <driver/i2c.h>

void* I2CDevice::BeginTransport()
{
    auto linker = i2c_cmd_link_create();
    auto err = i2c_master_start(linker);
    if (ESP_OK != err)
    {
       Serial.printf("I2C Start Error : %x \n",err);
       return nullptr;
    }
    return linker;
};
bool I2CDevice::EndTransport(void* linker,int i2cPort ,uint outTime)
{
    i2c_master_stop(linker);
    auto err = i2c_master_cmd_begin(i2cPort, linker, outTime);
    if (ESP_OK != err)
    {
        Serial.printf("I2C Begin Error : %x \n",err);
        i2c_cmd_link_delete(linker);
        return false;
    }
    i2c_cmd_link_delete(linker);
    return true;
};