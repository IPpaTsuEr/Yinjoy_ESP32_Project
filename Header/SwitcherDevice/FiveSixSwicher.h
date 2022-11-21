#pragma once
#include "../Interface/IDevice.h"
#include "../Service/GIOPInterruptService.h"

struct FiveSixSwicherPortConfig
{
    uint32_t Up, Down, Left, Right, Mid, Set, Rst;
    bool Positive_Enable;
};

EventGroupHandle_t _event_handel;

static void switcher_handle(void *port)
{
    if (port == nullptr)
    {
        //Serial.printf("按钮中断 值异常\n");
    }
    else
    {
        uint32_t data = (uint32_t)port;
        //Serial.printf("按钮中断 %d\n", data);
        xEventGroupSetBits(_event_handel, 1 << data);
    }
};
class FiveSixSwitcher : public IDevice
{
    FiveSixSwicherPortConfig _config;
    gpio_config_t ioconfig{};

public:
    FiveSixSwitcher(FiveSixSwicherPortConfig config) : _config(config)
    {
        GPIOInterruptService::initService();

        ioconfig.intr_type = gpio_int_type_t::GPIO_INTR_NEGEDGE;//GPIO_INTR_ANYEDGE
        ioconfig.pin_bit_mask = (1 << config.Up) |
                                (1 << config.Down) |
                                (1 << config.Left) |
                                (1 << config.Right) |
                                (1 << config.Mid) |
                                (1 << config.Set) |
                                (1 << config.Rst);
        ioconfig.mode = gpio_mode_t::GPIO_MODE_INPUT;
        ioconfig.pull_up_en = gpio_pullup_t::GPIO_PULLUP_ENABLE;
        ioconfig.pull_down_en = gpio_pulldown_t::GPIO_PULLDOWN_ENABLE;
        gpio_config(&ioconfig);

        //gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
        gpio_isr_handler_add((gpio_num_t)config.Up, switcher_handle, (void *)config.Up);
        gpio_isr_handler_add((gpio_num_t)config.Down, switcher_handle, (void *)config.Down);
        gpio_isr_handler_add((gpio_num_t)config.Left, switcher_handle, (void *)config.Left);
        gpio_isr_handler_add((gpio_num_t)config.Right, switcher_handle, (void *)config.Right);
        gpio_isr_handler_add((gpio_num_t)config.Mid, switcher_handle, (void *)config.Mid);
        gpio_isr_handler_add((gpio_num_t)config.Set, switcher_handle, (void *)config.Set);
        gpio_isr_handler_add((gpio_num_t)config.Rst, switcher_handle, (void *)config.Rst);

        _event_handel = xEventGroupCreate();
    };
    ~FiveSixSwitcher()
    {
        gpio_isr_handler_remove((gpio_num_t)_config.Up);
        gpio_isr_handler_remove((gpio_num_t)_config.Down);
        gpio_isr_handler_remove((gpio_num_t)_config.Left);
        gpio_isr_handler_remove((gpio_num_t)_config.Right);
        gpio_isr_handler_remove((gpio_num_t)_config.Mid);
        gpio_isr_handler_remove((gpio_num_t)_config.Set);
        gpio_isr_handler_remove((gpio_num_t)_config.Rst);

        if (_event_handel != nullptr)
        {
            vEventGroupDelete(_event_handel);
        }
    };

    EventGroupHandle_t GetEventHandle()
    {
        return _event_handel;
    };
    int64_t GetMaskBits()
    {
        return ioconfig.pin_bit_mask;
    }
    DviceProtocolType GetDeviceProtocolType() override
    {
        return DviceProtocolType::Other;
    };
    DeviceType GetDeviceType() override
    {
        return DeviceType::Switcher;
    };
    uint8_t GetDeviceID() override
    {
        return 0xf5;
    };
    String GetDeviceName() override
    {
        return "五向按钮";
    };
    String GetJsonString() override
    {
        return "";
    };
};