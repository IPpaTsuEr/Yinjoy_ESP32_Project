#include "SPIDevice.h"
#include <driver/sdspi_host.h>
#include <sdmmc_cmd.h>
class SPISDDevice : protected SPIDevice
{
protected:
    sdmmc_host_t _SD_Host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t _Device_Cofig = SDSPI_DEVICE_CONFIG_DEFAULT();

public:
    SPISDDevice(spi_host_device_t Host_Slot,
                gpio_num_t In_Port,
                gpio_num_t Out_Port,
                gpio_num_t CS_Port,
                gpio_num_t SCLK_Port) : SPIDevice(Host_Slot, In_Port, Out_Port, SCLK_Port, 4000, 3)
    {
        _SD_Host.slot = Host_Slot;
        _SD_Host.max_freq_khz =  SDMMC_FREQ_PROBING;
        _Device_Cofig.gpio_cs = CS_Port;
        _Device_Cofig.host_id = Host_Slot;
        //_Device_Cofig.width = 1;
    };

    virtual ~SPISDDevice(){};

    DviceProtocolType GetDeviceProtocolType() override
    {
        return DviceProtocolType::SPI;
    };
    DeviceType GetDeviceType() override
    {
        return DeviceType::Storage;
    };
    uint8_t GetDeviceID() override
    {
        return (uint8_t)0xf7;
    };
    String GetDeviceName() override
    {
        return "SD_Card_1";
    };

    String GetJsonString() override
    {
        return "{\"Name\":\"SD_Card_1\"}";
    };
};