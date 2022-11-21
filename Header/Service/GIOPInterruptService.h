#pragma once
struct GPIORegisterConf
{
    int Port;
    gpio_isr_t Handel;
    void *Data;
};

class GPIOInterruptService
{
private:
    static bool IsInited;

public:
    static void initService()
    {
        if (!IsInited)
        {
            IsInited = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1) == ESP_OK;
        }
    };
    static void Register(GPIORegisterConf registerConfig[], uint registerSize, gpio_config_t *ioconfig)
    {
        gpio_config(ioconfig);
        for (size_t i = 0; i < registerSize; i++)
        {
            gpio_isr_handler_add((gpio_num_t)registerConfig[i].Port, registerConfig[i].Handel, registerConfig[i].Data);
        }
    };

    static void UnRegister(GPIORegisterConf registerConfig[], uint registerSize)
    {
        for (size_t i = 0; i < registerSize; i++)
        {
            gpio_isr_handler_remove((gpio_num_t)registerConfig[i].Port);
        }
    };

    static void UnRegister(int port)
    {
        gpio_isr_handler_remove((gpio_num_t)port);
    };
};

bool GPIOInterruptService::IsInited = false;