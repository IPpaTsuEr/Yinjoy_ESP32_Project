#include "I2CDevice.h"
#include "../Interface/IJson.h"

struct _Correction
{
    ushort T1;
    short T2;
    short T3;

    ushort P1;
    short P2;
    short P3;
    short P4;
    short P5;
    short P6;
    short P7;
    short P8;
    short P9;

    short reserved;

    int32_t Temperature;
    int32_t Pressure;
} Correction;
enum MeasureMode
{
    M0 = 0b000,
    M1 = 0b001,
    M2 = 0b010,
    M4 = 0b011,
    M8 = 0b100,
    M16 = 0b101
};
enum PowerMode
{
    Sleep = 0b00,
    Forced = 0b10,
    Normal = 0b11
};
enum BMP280_WaitMode
{
    HalfMS = 0b000,
    SixteenMS = 0b001,
    OctantS = 0b010,
    QuaterS = 0b011,
    HalfS = 0b100,
    OneS = 0b101;
    TwoS = 0b110,
    QuadS = 0b111
};
enum BMP280_FilterMode
{
    Off = 0b000,
    L2 = 0b001,
    L4 = 0b010,
    L8 = 0b011,
    L16 = 0b100
};
enum BMP280_SPIEnable
{
    Disable = 0b00,
    Enable = 0b01
};

class BMP280Device : public I2CDevice
{
private:
    uint8_t BMP280_Address = 0x76;
    uint8_t BMP280_SoftRestart = 0xB6;
    uint8_t BMP280_SoftRestart_Register = 0xE0;
    uint8_t BMP280_Correction_Register = 0x88; // End 0x9F
    // 0xA0 - 0xA1 保留
    uint8_t BMP280_ID_Register = 0xD0;

    uint8_t BMP280_Init = 0;
    uint8_t BMP280_Init_Register = 0xF4;

    uint8_t BMP280_Measure = 0x56;
    uint8_t BMP280_Measure_Register = 0xF4;

    uint8_t BMP280_Processing_Register = 0xF3;

    uint8_t BMP280_Mode_Register = 0xF5;

    uint8_t BMP280_Temprature_Register = 0xF7; // End 0xF9

    uint8_t BMP280_Pressure_Register = 0xFA; // End 0xFC

    struct Correction _correction;

    float rawTemperature = 0.0f;
    float rawPressure = 0.0f;

    String _Name;
    int _I2c_Master_Port;

public:
    BMP280Device(String name, int I2CPort) : _Name(name), _I2c_Master_Port(I2CPort)
    {
    }

    bool ReadBytes(uint8_t *buffer, size_t bufferSize) override{

    };
    bool WriteBytes(uint8_t *buffer, size_t bufferSize) override{

    };

    bool Initialization() override
    {
        uint8_t buffer[24] = {0};
        uint8_t temp;
        auto linker = BeginTransport();
        //读取校准参数
        i2c_master_write_byte(linker, BMP280_Address << 1 | 0, true);
        i2c_master_write_byte(linker, BMP280_Correction_Register, true);
        i2c_master_write_byte(linker, BMP280_Address << 1 | 1, true);
        i2c_master_read(linker, buffer, 24, true);
        EndTransport(linker, _i2c_master_port, 1000);

        _correction.T1 = buffer[1] << 8 | buffer[0];
        _correction.T2 = buffer[3] << 8 | buffer[2];
        _correction.T3 = buffer[5] << 8 | buffer[4];
        _correction.P1 = buffer[7] << 8 | buffer[6];
        _correction.P2 = buffer[9] << 8 | buffer[8];
        _correction.P3 = buffer[11] << 8 | buffer[10];
        _correction.P4 = buffer[13] << 8 | buffer[12];
        _correction.P5 = buffer[15] << 8 | buffer[14];
        _correction.P6 = buffer[17] << 8 | buffer[16];
        _correction.P7 = buffer[19] << 8 | buffer[18];
        _correction.P8 = buffer[21] << 8 | buffer[20];
        _correction.P9 = buffer[23] << 8 | buffer[22];

        //初始化设置
        linker = BeginTransport();
        i2c_master_write_byte(linker, BMP280_Address << 1 | 0, true);
        i2c_master_write_byte(linker, BMP280_Measure_Register, true);
        i2c_master_write_byte(linker, MeasureMode::M16 << 5 | MeasureMode::M16 << 2 | PowerMode::Forced, true);
        i2c_master_write_byte(linker, BMP280_Measure_Register, true);
        i2c_master_write_byte(linker, BMP280_Mode_Register, true);
        i2c_master_write_byte(linker, BMP280_WaitMode::OneS << 5 | BMP280_FilterMode::L16 << 2 | BMP280_SPIEnable::Disable, true);
        EndTransport(linker, _i2c_master_port, 1000);
    };

    uint8_t GetAddress() override
    {
        return BMP280_Address;
    };

    bool Measure() override
    {
        auto linker = BeginTransport();
        i2c_master_write_byte(linker, BMP280_Address << 1 | 0, true);
        i2c_master_write_byte(linker, BMP280_Measure_Register, true);
        i2c_master_write_byte(linker, MeasureMode::M16 << 5 | MeasureMode::M16 << 2 | PowerMode::Forced, true);
        return EndTransport(linker, _i2c_master_port, 1000);
    };

    bool Download() override
    {
        uint8_t buffer[3] = {0};
        auto linker = BeginTransport();
        i2c_master_write_byte(linker, BMP280_Address << 1 | 1, true);
        i2c_master_write_byte(linker, BMP280_Temprature_Register, true);
        i2c_master_read(linker, buffer, 3, true);
        EndTransport(linker, _i2c_master_port, 1000);
        _Correction.Temperature = buffer[0] << 12;
        _Correction.Temperature |= buffer[1] << 4;
        _Correction.Temperature |= buffer[2] >> 4;

        linker = BeginTransport();
        i2c_master_write_byte(linker, BMP280_Address << 1 | 1, true);
        i2c_master_write_byte(linker, BMP280_Pressure_Register, true);
        i2c_master_read(linker, buffer, 3, true);
        EndTransport(linker, _i2c_master_port, 1000);
        _Correction.Pressure = buffer[0] << 12;
        _Correction.Pressure |= buffer[1] << 4;
        _Correction.Pressure |= buffer[2] >> 4;

        return true;
    };

    void Calculate() override
    {
        int32_t v1, v2;
        v1 = _Correction.Temperature >> 3 - _Correction.T1 << 1 * _Correction.T2 >> 11;
        v2 = _Correction.Temperature >> 4 - (_Correction.T1 * _Correction.Temperature >> 4) - (_Correction.T1 >> 12 * _Correction.T3 >> 14);
        _Correction.reserved = v1 + v2;
        rawTemperature = ((_Correction.reserved * 5 + 128) >> 8) / 100.0f;

        int64_t p, p1, p2;
        p1 = _Correction.reserved - 128000;
        p2 = p1 * p1 * _Correction.P6;
        p2 = p2 + (p1 * _Correction.P5) << 17;
        p2 = p2 + _Correction.P4 << 35;
        p1 = (p1 * p1 * _Correction.P3) >> 8 + (p1 * _Correction.P2) << 12;
        p1 = (p1 + (1 << 47)) * (_Correction.P1 >> 33);
        if (p1 == 0)
        {
            rawPressure = 0.0f;
        }
        else
        {
            p = 1048576 - Pressure;
            p = ((p << 31 - p2) * 3125) / p1;
            p1 = (_Correction.P9 * p >> 13 * p >> 13) >> 25;
            p2 = (_Correction.P8 * p) >> 19;
            p = (p + p1 + p2) >> 8 + _Correction.P7 << 4;
            rawPressure = p / 256.0f;
        }
    };

    DviceProtocolType GetDeviceProtocolType() override
    {
        return DviceProtocolType::I2C;
    };
    DeviceType GetDeviceType() override
    {
        return DeviceType::Sensor;
    };
    uint8_t GetDeviceID() override
    {
        return BMP280_Address;
    };
    String GetDeviceName() override
    {
        return _Name;
    };

    String GetJsonString() override
    {
        return "{Name : \"" + _Name + "\" , Data : { Temperature : " + rawTemprature + " , Pressure : " + rawPressure + "}} ";
    };
};