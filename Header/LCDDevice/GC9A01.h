#pragma once
#include "../Service/MultipleBufferService.h"
#include "../Service/SPIDMAService.h"

DRAM_ATTR static const spi_dma_transform_struct InitData[54] = {
    {0xEF, {0}, 0},
    {0xEB, {0x14}, 1},
    {0xFE, {0}, 0},
    {0xEF, {0}, 0},
    {0xEB, {0x14}, 1},
    {0x84, {0x40}, 1},
    {0x85, {0xFF}, 1},
    {0x86, {0xFF}, 1},
    {0x87, {0xFF}, 1},
    {0x88, {0x0A}, 1},
    {0x89, {0x21}, 1},
    {0x8A, {0x00}, 1},
    {0x8B, {0x80}, 1},
    {0x8C, {0x01}, 1},
    {0x8D, {0x01}, 1},
    {0x8E, {0xFF}, 1},
    {0x8F, {0xFF}, 1},
    {0xB6, {0x00, 0x00}, 2},
    {0x36, {0x48}, 1},
    {0x3A, {0x06}, 1},
    {0x90, {0x08, 0x08, 0x08, 0x08}, 4},
    {0xBD, {0x06}, 1},
    {0xBC, {0x00}, 1},
    {0xFF, {0x60, 0x01, 0x04}, 1},
    {0xC3, {0x13}, 1},
    {0xC4, {0x13}, 1},
    {0xC9, {0x22}, 1},
    {0xBE, {0x11}, 1},
    {0xE1, {0x10, 0x0E}, 1},
    {0xDF, {0x21, 0x0C, 0x02}, 3},
    {0xF0, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}, 6},
    {0xF1, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}, 6},
    //{0x8A, {0x00}, 1},
    {0xF2, {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A}, 6},
    {0xF3, {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F}, 6},
    {0xED, {0x1B, 0x0B}, 2},
    {0xAE, {0x77}, 1},
    {0xCD, {0x63}, 1},
    {0x70, {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03}, 9},
    {0xE8, {0x34}, 1},
    {0x62, {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70}, 12},
    {0x63, {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70}, 12},
    {0x64, {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07}, 7},
    {0x66, {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00}, 10},
    {0x67, {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98}, 10},
    {0x74, {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00}, 7},
    {0x98, {0x3e, 0x07}, 2},
    {0x35, {0}, 0},
    {0x21, {0}, 0},
    {0x11, {0}, 0},
    {0x00, {120}, 0xFE},
    {0x29, {0}, 0},
    {0x00, {20}, 0xFE},
    {0x00, {0}, 0xFF},
};

DRAM_ATTR static const spi_dma_transform_struct FramData[3] = {
    {0x2A, {0x0, 0x0, (uint8_t)(((240 - 1) >> 8) & 0xFF), (uint8_t)((240 - 1) & 0xFF)}, 4},
    {0x2B, {0x0, 0x0, (uint8_t)(((240 - 1) >> 8) & 0xFF), (uint8_t)((240 - 1) & 0xFF)}, 4},
    {0x00, {0x0}, 0xFF},
};

class GC9A01 : public SPIDevice
{
private:
    gpio_num_t _CS;
    gpio_num_t _DC;
    gpio_num_t _RES;
    uint8_t _Width, _Height;
    uint _BufferLineSize;
    MultipleBufferService *_buffer_service;
    SPIDMAService *_spi_dma_service;
    int dma_channel = 2;

#ifdef _NTTR
    spi_transaction_t image_trans[6];

    void WriteImage(uint buffer_index, uint image_line_index)
    {
        memset(&image_trans[0], 0, sizeof(spi_transaction_t));
        memset(&image_trans[1], 0, sizeof(spi_transaction_t));
        memset(&image_trans[2], 0, sizeof(spi_transaction_t));
        memset(&image_trans[3], 0, sizeof(spi_transaction_t));
        memset(&image_trans[4], 0, sizeof(spi_transaction_t));
        memset(&image_trans[5], 0, sizeof(spi_transaction_t));
        // colum Range Set
        image_trans[0].length = 8;
        image_trans[0].user = (void *)0;
        image_trans[0].tx_data[0] = 0x2A;
        image_trans[0].flags = SPI_TRANS_USE_TXDATA;

        image_trans[1].length = 32;
        image_trans[1].user = (void *)1;
        image_trans[1].tx_data[0] = 0;
        image_trans[1].tx_data[1] = 0;
        image_trans[1].tx_data[2] = (uint8_t)((_Width - 1) >> 8 & 0xFF);
        image_trans[1].tx_data[3] = (uint8_t)((_Width - 1) & 0xFF);
        image_trans[1].flags = SPI_TRANS_USE_TXDATA;

        // Col set
        image_trans[2].length = 8;
        image_trans[2].user = (void *)0;
        image_trans[2].tx_data[0] = 0x2B;
        image_trans[2].flags = SPI_TRANS_USE_TXDATA;

        image_trans[3].length = 32;
        image_trans[3].user = (void *)1;
        image_trans[3].tx_data[0] = 0;
        image_trans[3].tx_data[1] = 0;
        image_trans[3].tx_data[2] = (uint8_t)((_Height - 1) >> 8 & 0xFF);
        image_trans[3].tx_data[3] = (uint8_t)((_Height - 1) & 0xFF);
        image_trans[3].flags = SPI_TRANS_USE_TXDATA;

        // image data
        image_trans[4].length = 8;
        image_trans[4].user = (void *)0;
        image_trans[4].tx_data[0] = (image_line_index == 0) ? (uint8_t)0x2C : (uint8_t)0x3C;
        image_trans[4].flags = SPI_TRANS_USE_TXDATA;

        image_trans[5].length = _BufferLineSize;
        image_trans[5].user = (void *)1;
        image_trans[5].tx_buffer = _buffer_service->GetAddress(buffer_index);
        image_trans[5].flags = 0;
        assert(ESP_OK == gpio_set_level(_CS, 0));
        _spi_dma_service->spi_dma_queue_transform(image_trans, 6);
        assert(ESP_OK == gpio_set_level(_CS, 1));
        _spi_dma_service->wait_spi_dma_queue_transform_finish(6);
        Serial.printf("写图像行行数据完毕 %d , %d \n", buffer_index, image_line_index);
    };
#else
    spi_transaction_t image_trans;
    void WriteImage(uint buffer_index, uint image_line_index)
    {
        Serial.printf("写图像行行数据\n");
        memset(&image_trans, 0, sizeof(spi_transaction_t));
        if (image_line_index == 0)
        {
            _spi_dma_service->spi_dma_send_cmd((image_line_index == 0) ? (uint8_t)0x2C : (uint8_t)0x3C);
            Serial.printf("写图像命令已发送");
        }
        auto ptr = _buffer_service->GetAddress(buffer_index);
        for (size_t i = 0; i < _BufferLineSize; i += 3)
        {
            _spi_dma_service->spi_dma_send_data((uint8_t *)ptr + i * 3, sizeof(uint8_t) * 3);
        }
        return;
        image_trans.length = _BufferLineSize * 8;
        image_trans.user = (void *)1;
        image_trans.tx_buffer = _buffer_service->GetAddress(buffer_index);
        image_trans.flags = SPI_TRANS_MODE_OCT; // SPI_TRANS_MODE_OCT;

        _spi_dma_service->spi_dma_queue_transform(&image_trans, 1);
        _spi_dma_service->wait_spi_dma_queue_transform_finish(1);

        Serial.printf("写图像行行数据完毕 %d , %d \n", buffer_index, image_line_index);
    };
#endif

    void Reset()
    {
        assert(ESP_OK == gpio_set_level(_CS, 1));
        delay(5);
        assert(ESP_OK == gpio_set_level(_RES, 0));
        delay(10);
        assert(ESP_OK == gpio_set_level(_RES, 1));
        delay(120);
        Serial.printf("重置设备完成\n");
    };
    void GetId()
    {
        Serial.printf("请求设备ID\n");
        spi_device_acquire_bus(_spi_dma_service->get_device_handle(), portMAX_DELAY);
        _spi_dma_service->spi_dma_send_cmd(0xDA, true);
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length = 8 * 4;
        t.flags = SPI_TRANS_USE_RXDATA;
        t.user = (void *)1;

        esp_err_t ret = spi_device_polling_transmit(_spi_dma_service->get_device_handle(), &t);
        assert(ret == ESP_OK);
        spi_device_release_bus(_spi_dma_service->get_device_handle());
        Serial.printf("取得设备ID0 %d \n", t.rx_data[0]);
        Serial.printf("取得设备ID1 %d \n", t.rx_data[1]);
        Serial.printf("取得设备ID2 %d \n", t.rx_data[2]);
        Serial.printf("取得设备ID3 %d \n", t.rx_data[3]);
    };

public:
    GC9A01(spi_host_device_t SPI_Slot,
           gpio_num_t In_Port,
           gpio_num_t Cs_Port,
           gpio_num_t Dc_Port,
           gpio_num_t Res_Port,
           gpio_num_t SCLK_Port,
           uint8_t Width,
           uint8_t Height) : SPIDevice(SPI_Slot, -1, In_Port, SCLK_Port, Width * 3, 3)
    {
        _CS = Cs_Port;
        _DC = Dc_Port;
        _RES = Res_Port;

        _Width = Width;
        _Height = Height;
        _BufferLineSize = Width * 3;
        Serial.println("开始初始化端口");
        assert(ESP_OK == gpio_set_direction(_RES, gpio_mode_t::GPIO_MODE_OUTPUT));
        Serial.println("_RES 端口 初始化 成功");
        assert(ESP_OK == gpio_set_direction(_DC, gpio_mode_t::GPIO_MODE_OUTPUT));
        Serial.println("_DC 端口 初始化 成功");
        gpio_pad_select_gpio(_CS);
        assert(ESP_OK == gpio_set_direction(_CS, gpio_mode_t::GPIO_MODE_OUTPUT));
        Serial.println("_CS 端口 初始化 成功");

        _buffer_service = new MultipleBufferService(_BufferLineSize, 2, MALLOC_CAP_DMA);
        assert(_buffer_service != nullptr);
        _spi_dma_service = new SPIDMAService(SPI_Slot, _CS, _DC, SPI_MASTER_FREQ_40M);
        assert(_spi_dma_service != nullptr);
    };

    void Initialization()
    {
        Reset();
        // GetId();
        _spi_dma_service->spi_dma_transform((spi_dma_transform_struct_t *)InitData);
        _spi_dma_service->spi_dma_transform((spi_dma_transform_struct_t *)FramData);
    };

    ~GC9A01()
    {
        gpio_set_level(_CS, 0);
        delete _spi_dma_service;
        delete _buffer_service;
    };

    DviceProtocolType GetDeviceProtocolType() override
    {
        return DviceProtocolType::SPI;
    };
    DeviceType GetDeviceType() override
    {
        return DeviceType::Display;
    };
    uint8_t GetDeviceID() override
    {
        return (uint8_t)0xA0;
    };
    String GetDeviceName() override
    {
        return "TFT_1.28_240*240";
    };
    String GetJsonString()
    {
        return "{\"Name\":\"TFT_1.28_240*240\"}";
    };

    void Test()
    {
        uint8_t color[3];

        // Triangle
        color[0] = 0xFF;
        color[1] = 0xFF;

        for (int y = 0; y < 240; y++)
        {
            for (int x = 0; x < 240; x++)
            {
                if (x < y)
                {
                    color[2] = 0xFF;
                }
                else
                {
                    color[2] = 0x00;
                }
                assert(true == _buffer_service->WriteBuffer(y % 2, color, sizeof(color), x * sizeof(color)));
            }
            WriteImage(y % 2, y);
        }

        delay(1000);

        // Rainbow
        float frequency = 0.026;

        for (int y = 0; y < 240; y++)
        {
            for (int x = 0; x < 240; x++)
            {
                color[0] = sin(frequency * x + 0) * 127 + 128;
                color[1] = sin(frequency * x + 2) * 127 + 128;
                color[2] = sin(frequency * x + 4) * 127 + 128;
                assert(true == _buffer_service->WriteBuffer(y % 2, color, sizeof(color), x * sizeof(color)));
            }
            WriteImage(y % 2, y);
        }

        delay(1000);

        // Checkerboard
        for (int y = 0; y < 240; y++)
        {
            for (int x = 0; x < 240; x++)
            {

                if ((x / 10) % 2 == (y / 10) % 2)
                {
                    color[0] = 0xFF;
                    color[1] = 0xFF;
                    color[2] = 0xFF;
                }
                else
                {
                    color[0] = 0x00;
                    color[1] = 0x00;
                    color[2] = 0x00;
                }
                assert(true == _buffer_service->WriteBuffer(y % 2, color, sizeof(color), x * sizeof(color)));
            }
            WriteImage(y % 2, y);
        }

        delay(1000);

        // Swiss flag
        color[0] = 0xFF;
        for (int y = 0; y < 240; y++)
        {
            for (int x = 0; x < 240; x++)
            {

                if ((x >= 1 * 48 && x < 4 * 48 && y >= 2 * 48 && y < 3 * 48) ||
                    (x >= 2 * 48 && x < 3 * 48 && y >= 1 * 48 && y < 4 * 48))
                {
                    color[1] = 0xFF;
                    color[2] = 0xFF;
                }
                else
                {
                    color[1] = 0x00;
                    color[2] = 0x00;
                }
                assert(true == _buffer_service->WriteBuffer(y % 2, color, sizeof(color), x * sizeof(color)));
            }
            WriteImage(y % 2, y);
        }

        delay(1000);
    }
    void TestRender()
    {
        uint buffer_index = 0;
        bool isFirst = true;
        uint line_index = 0;
        uint frame_index = 0;
        while (true)
        {
            if (isFirst)
                buffer_index = 0;
            else
                buffer_index = 1;
            auto type = frame_index % 6;
            auto ptr = _buffer_service->GetAddress(buffer_index);
            switch (type)
            {
            case 0:
                memset(ptr, 0xD2, _BufferLineSize);
                break;

            case 1:
                memset(ptr, 0xA8, _BufferLineSize);
                break;

            case 2:
                memset(ptr, 0x7E, _BufferLineSize);
                break;

            case 3:
                memset(ptr, 0x54, _BufferLineSize);
                break;

            case 4:
                memset(ptr, 0x2A, _BufferLineSize);
                break;

            case 5:
                memset(ptr, 0xFC, _BufferLineSize);
                break;
            default:
                break;
            }

            WriteImage(buffer_index, line_index);

            Serial.printf("填充 %d 帧 %d 行\n", frame_index, line_index);

            line_index++;
            line_index %= _Height;
            if (line_index == 0)
            {
                frame_index++;
                vTaskDelay(1000);
            }
            isFirst = !isFirst;
        }
    };
};