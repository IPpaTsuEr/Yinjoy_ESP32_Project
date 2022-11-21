#pragma once
#include <esp32-hal-spi.h>
#include <driver/spi_common.h>
#include "../Interface/IDevice.h"

class SPIDevice : public IDevice
{
protected:
#ifdef __cplusplus
    spi_dma_chan_t SPI_DMI_CHAN;
#else
    int SPI_DMI_CHAN;
#endif
    spi_host_device_t _SPI_Slot;

    spi_bus_config_t bus_config = {
        .mosi_io_num = 0,
        .miso_io_num = 0,
        .sclk_io_num = 0,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000
    };

public:
    SPIDevice(spi_host_device_t SPI_Slot,
              int In_Port,
              int Out_Port,
              int SCLK_Port,
              int Transfer_Size = 4000,
              int dma_channel = 0) : _SPI_Slot(SPI_Slot)
    {
        bus_config.miso_io_num = In_Port;
        bus_config.mosi_io_num = Out_Port;
        bus_config.sclk_io_num = SCLK_Port;
        bus_config.max_transfer_sz = Transfer_Size;
        Serial.printf("使用DMA %d \n",dma_channel);
        auto res = spi_bus_initialize(SPI_Slot, &bus_config, dma_channel);
        assert(res == ESP_OK);
        Serial.println("SPIDevice 初始化成功");
    };

    SPIDevice(spi_host_device_t SPI_Slot) : _SPI_Slot(SPI_Slot)
    {
        spi_bus_initialize(SPI_Slot, &bus_config, 0);
    };

    SPIDevice(spi_host_device_t SPI_Slot, int dma_channel = 2) : _SPI_Slot(SPI_Slot)
    {
        SPI_DMI_CHAN = dma_channel;
        spi_bus_initialize(SPI_Slot, &bus_config, dma_channel);
    };

    virtual ~SPIDevice()
    {
        spi_bus_free(_SPI_Slot);
    };
};