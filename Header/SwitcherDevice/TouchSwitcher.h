#pragma once
#include "../Interface/IDevice.h"

#include "../Service/GIOPInterruptService.h"

class TouchSwitcher : public IDevice
{

    uint port;
    gpio_config_t ioconfig{};
    GPIORegisterConf gpioConfi{};

public:
    TouchSwitcher(int port)
    {
        this.port = port;
        GPIOInterruptService::initService();

        ioconfig.intr_type = gpio_int_type_t::GPIO_INTR_NEGEDGE; // GPIO_INTR_ANYEDGE
        ioconfig.pin_bit_mask = 1 << port;
        ioconfig.mode = gpio_mode_t::GPIO_MODE_INPUT;
        ioconfig.pull_up_en = gpio_pullup_t::GPIO_PULLUP_ENABLE;
        ioconfig.pull_down_en = gpio_pulldown_t::GPIO_PULLDOWN_ENABLE;
        
        gpioConfi.Port = port;
        gpioConfi.Data = (void *)port;
        gpioConfi.Handel = nullptr;

        GPIOInterruptService::Register(&gpioConfi, 1, ioconfig);
    };
    ~TouchSwitcher()
    {

        GPIOInterruptService::UnRegister(port);
    };
}