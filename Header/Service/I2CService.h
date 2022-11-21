#pragma once
#include <driver/i2c.h>
#include <list>

class I2CService
{
private:
    static i2c_port_t _i2c_master_port;

public:
    static i2c_port_t Init(gpio_num_t SDA_Port = GPIO_NUM_4, gpio_num_t SCL_Port = GPIO_NUM_5, uint32_t I2C_Frequence_Hz = 400000)
    {

        i2c_config_t i2cConfigr = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = SDA_Port,
            .scl_io_num = SCL_Port,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            //.clk_flags = 0
        };
        i2cConfigr.master.clk_speed = I2C_Frequence_Hz;
        auto result = i2c_param_config(_i2c_master_port, &i2cConfigr);
        result = i2c_driver_install(_i2c_master_port, i2cConfigr.mode, 0, 0, 0);
        if (result != ESP_OK)
        {
            Serial.printf("I2C 初始化失败 , ErrorCode = %d \n", result);
            return -1;
        }
        else
        {
            Serial.println("I2C 初始化成功");
            _i2c_master_port++;
            return _i2c_master_port;
        }
    };


    static std::list<int>* QueryDeviceAddress(i2c_port_t Master_port)
    {
        std::list<int> *result = new std::list<int>();
        for (size_t i = 1; i < 127;)
        {
            auto linker = i2c_cmd_link_create();
            auto err = i2c_master_start(linker);
            if (err != ESP_OK)
                continue;
            if (linker != nullptr)
            {
                err = i2c_master_write_byte(linker, (i << 1) | 1, true);
                if (err != ESP_OK)
                    continue;
                err = i2c_master_stop(linker);
                if (err != ESP_OK)
                    continue;
                err = i2c_master_cmd_begin(Master_port, linker, 1000);
                if (err == ESP_OK)
                {
                    result->push_back(i);
                }
            }
        }
        return result;
    };
};

i2c_port_t I2CService::_i2c_master_port = 0;