
#include "Header/I2CDevice/AHT21BDevice.h"

bool AHT21B::ReadBytes(uint8_t *buffer, size_t bufferSize)
{
    auto linker = BeginTransport();
    i2c_master_write_byte(linker, (_address << 1) | DHT21B_Read, true);

    bool _dataReady = false;
    uint8_t data = 0;
    do
    {
        i2c_master_read_byte(linker, &data, I2C_MASTER_ACK);
        //#ifdef __Debug
        Serial.printf("读取到状态数据 => %02X \n", data);
        Serial.println(data, BIN);
        //#endif
        if (data & DHT21B_IsInited != DHT21B_IsInited)
        {
            Serial.println("Device Need Init");
            NeedInitDevice = true;
            Initialization();
            break;
        }
        else
        {
            NeedInitDevice = false;
        }
        //设备忙
        if (data & DHT21B_IsBusy == DHT21B_IsBusy)
        {
            Serial.println("Device Is Busy");
            delay(DHT21B_MesureWaitTime);
            break;
        }
        else
        {
            //设备已完成测量
            _dataReady = true;
            break;
        }
    } while (true);

    if (_dataReady)
    {
        for (size_t i = 0; i < bufferSize; i++)
        {
            //读取测量数据
            if (i + 1 == bufferSize)
            {
                i2c_master_read_byte(linker, &buffer[i], I2C_MASTER_NACK);
            }
            else
            {
                i2c_master_read_byte(linker, &buffer[i], I2C_MASTER_ACK);
            }
            //#ifdef __Debug
            Serial.printf("读取到数据 %d => %d %02X \n", i, buffer[i]);
            //#endif
        }
    }
    auto err = EndTransport(linker, _i2cPort, _outTime);
    Serial.printf("Read Result = %s \n", err == ESP_OK ? "True" : "False");
    return _dataReady && err == ESP_OK;
}

bool AHT21B::WriteBytes(uint8_t *buffer, size_t bufferSize)
{
    auto linker = BeginTransport();
    i2c_master_write_byte(linker, (_address << 1) | DHT21B_Write, true);
    for (size_t i = 0; i < bufferSize; i++)
    {
        i2c_master_write_byte(linker, buffer[i], true);
    }
    auto err = EndTransport(linker, _i2cPort, _outTime);
    Serial.printf("Write Result = %s \n", err == ESP_OK ? "True" : "False");
    return err == ESP_OK;
}

bool AHT21B::Initialization()
{
    auto linker = BeginTransport();
    i2c_master_write_byte(linker, (_address << 1) | DHT21B_Read, true);
    uint8_t buffer;
    i2c_master_read_byte(linker, &buffer, I2C_MASTER_ACK);

    Serial.printf("Init 读取到状态数据 => %02X \n", buffer);
    Serial.println(buffer, BIN);

    auto err = EndTransport(linker, _i2cPort, _outTime);

    if (buffer & DHT21B_IsInited != DHT21B_IsInited)
    {
        Serial.printf("Init 初始化");
        linker = BeginTransport();
        i2c_master_write_byte(linker, (_address << 1) | DHT21B_Write, true);
        for (size_t i = 0; i < 3; i++)
        {
            i2c_master_write_byte(linker, InitCmdData[i], true);
        }

        err = EndTransport(linker, _i2cPort, _outTime);
    }
    Serial.printf("Init Result = %s \n", err == ESP_OK ? "True" : "False");
    return err == ESP_OK;
}

inline uint8_t AHT21B::GetAddress() { return _i2cPort; }

String AHT21B::GetJsonString()
{
    auto data = "{\"Name\":\"" + _name +"\",\"Data\":{\"WenDu\":" + _temp + ",\"ShiDu\":" + _shidu + "}}";
    Serial.println(data);
    return data;
}

void AHT21B::Calculate()
{
    float_t _data = 0.0f;
    _data = (MesureResultData[2] & 0xf) << 16 | MesureResultData[3] << 8 | MesureResultData[4];
    _temp = (_data / 1048576.0f) * 200.0f - 50;
    _data = (MesureResultData[0] << 16 | MesureResultData[1] << 8 | MesureResultData[2]) >> 4;
    _shidu = (_data * 100.0f) / 1048576;
}
