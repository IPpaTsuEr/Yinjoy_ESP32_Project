#pragma once
#include "../SPIDevice/SPIDevice.h"

#define spi_dma_transform_end 0xFF
#define spi_dma_transform_delay 0xFE
#define spi_dma_transform_type_cmd 0
#define spi_dma_transform_type_data 1
#define spi_dma_transform_data_max_length 12

typedef struct spi_dma_transform_struct
{
    uint8_t cmd;
    uint8_t data[spi_dma_transform_data_max_length];
    uint8_t data_length;
} spi_dma_transform_struct_t;

class SPIDMAService
{
    static gpio_num_t _DC;
    static gpio_num_t _CS;

    spi_host_device_t _slot;
    spi_device_handle_t _handle;
    spi_device_interface_config_t _device_config = {
        .mode = 0,
        .clock_speed_hz = 40 * 1000 * 1000,
        .spics_io_num = -1,
        .queue_size = 7,
        .pre_cb = &SPIDMAService::dma_transform_callback,
        .post_cb = &SPIDMAService::dma_transform_end_callback};

private:
    static void dma_transform_callback(spi_transaction_t *t)
    {
        int DC = (int)t->user;
        // if (DC == 0)
        // Serial.println("命令状态位");
        // else
        // Serial.println("数据状态位");

        gpio_set_level(SPIDMAService::_DC, DC);
        gpio_set_level(SPIDMAService::_CS, 0);
    };
    static void dma_transform_end_callback(spi_transaction_t *t)
    {
        //int DC = (int)t->user;
        //gpio_set_level(SPIDMAService::_DC, DC == 1 ? 0 : 1);
        gpio_set_level(SPIDMAService::_CS, 1);
    };

public:
    SPIDMAService(spi_host_device_t spi_slot, gpio_num_t cs, gpio_num_t dc, int clock_speed)
    {
        SPIDMAService::_DC = dc;
        SPIDMAService::_CS = cs;
        Serial.printf("DC 端口被设置为 %d \n", SPIDMAService::_DC);
        _slot = spi_slot;
        _device_config.spics_io_num = cs;
        _device_config.clock_speed_hz = clock_speed;
        auto res = spi_bus_add_device(spi_slot, &_device_config, &_handle);
        assert(res == ESP_OK);
        Serial.println("SPIDMAService 初始化成功");
    };

    ~SPIDMAService(){};

    spi_device_handle_t get_device_handle()
    {
        return _handle;
    };
    spi_host_device_t get_host_device()
    {

        return _slot;
    };

    void spi_dma_send_cmd(uint8_t cmd_data, bool keep_active = false)
    {
        if (cmd_data == 0x00)
            return;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 8;
        t.tx_buffer = &cmd_data;
        t.user = (void *)spi_dma_transform_type_cmd;
        t.flags = 0;//SPI_TRANS_MODE_OCT;
        if (keep_active)
            t.flags |= SPI_TRANS_CS_KEEP_ACTIVE;
        Serial.printf("spi_dma_send_cmd 正发送指令 %d \n", cmd_data);

        auto result = spi_device_polling_transmit(_handle, &t);
        assert(result == ESP_OK);
    };
    void spi_dma_send_data(uint8_t *cmd_data, uint data_size)
    {
        if (data_size == 0 || data_size == 0xFF || data_size == 0xFE)
            return;
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 8 * data_size;
        t.tx_buffer = cmd_data;
        t.user = (void *)spi_dma_transform_type_data;
        t.flags = 0;//SPI_TRANS_MODE_OCT;
        Serial.printf("spi_dma_send_cmd 正发送数据 %d \n", cmd_data[0]);

        auto result = spi_device_polling_transmit(_handle, &t);
        assert(result == ESP_OK);
    };
    void spi_dma_transform(spi_dma_transform_struct_t *datas)
    {
        uint index = 0;
        while (true)
        {
            auto current = datas[index];
            if (current.data_length == spi_dma_transform_end)
            {
                Serial.println("spi_dma_transform 已执行到命令列表末尾");
                break;
            }
            if (current.data_length == spi_dma_transform_delay)
            {
                Serial.printf("spi_dma_transform 已执行到等待 %d \n", current.data[0]);
                vTaskDelay(current.data[0] / portTICK_PERIOD_MS);
                index++;
                continue;
            }
            spi_dma_send_cmd(current.cmd);
            spi_dma_send_data(current.data, current.data_length);
            index++;
        }
    };
    void spi_dma_queue_transform(spi_transaction_t *trans, uint size, TickType_t max_wait_tick = portMAX_DELAY)
    {
        if (size == 1)
        {
            auto ret = spi_device_queue_trans(_handle, trans, max_wait_tick);
            assert(ret == ESP_OK);
        }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                auto ret = spi_device_queue_trans(_handle, &trans[i], max_wait_tick);
                assert(ret == ESP_OK);
            }
        }
    };

    void wait_spi_dma_queue_transform_finish(uint wait_count, TickType_t max_wait_tick = portMAX_DELAY)
    {
        spi_transaction_t *tra;
        while (wait_count > 0)
        {
            auto res = spi_device_get_trans_result(_handle, &tra, max_wait_tick);
            assert(res == ESP_OK);
            wait_count--;
        }
    };
};
gpio_num_t SPIDMAService::_DC = GPIO_NUM_6;

gpio_num_t SPIDMAService::_CS = GPIO_NUM_7;