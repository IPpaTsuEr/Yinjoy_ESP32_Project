
#define _MY_SD

#ifdef _MY_LCD
// LCD
#include <esp_task.h>
#include <freertos/task.h>

#include "Header/Service/SDCardService.h"
#include "Header/LCDDevice/GC9A01.h"
GC9A01 *lcd;
void setup()
{
    Serial.begin(115200);
    lcd = new GC9A01(SPI2_HOST,
                     GPIO_NUM_3,
                     GPIO_NUM_7,
                     GPIO_NUM_6,
                     GPIO_NUM_11,
                     GPIO_NUM_2,
                     240, 240);
    lcd->Initialization();
}
void loop()
{
    // lcd->Test();
    lcd->TestRender();
}
#endif
#ifdef _MY_PWM
#include <esp_task.h>
#include <freertos/task.h>
#include "Header/PWMDevice/PWMDevice.h"
// pwm led test *****
PWMDevice *m_led = new PWMDevice();
// pwm led test *****

void setup()
{
    Serial.begin(115200);
    m_led->HardwareStart(4095);
}
void loop()
{
    delay(1500);
    if (isa)
        m_led->HardwareStart(4095 * 2 - 1);
    else
        m_led->HardwareStart(0);

    isa = !isa;
}
#endif

#ifdef _MY_I2C

//设备测量 *********
#include <driver/i2c.h>
#include <esp_task.h>
#include <freertos/task.h>
#include "Header/I2CDevice/AHT21BDevice.h"
#include "Header/Service/I2CService.h"

int i2c_master_port = 0;
AHT21B *_aht21b;

void setup()
{
    Serial.begin(115200);
    auto i2c_master_port = I2CService::Init();
    if (i2c_master_port != -1)
    {
        delay(100);
        _aht21b = new AHT21B("温度湿度传感器", i2c_master_port);
        _aht21b->Initialization();
    }
}
void loop()
{
    I2CMeasure();
    Serial.println(_aht21b->GetJsonString());
    delay(300);
    Serial.println("Looping 。。。");
}

void I2CMeasure()
{
    delay(1000);
    if (!_aht21b->Measure())
    {
        delay(200);
        while (!_aht21b->Download())
        {
            delay(200);
        }
        _aht21b->Calculate();
    }
}
#endif

#ifdef _MY_INTERRUPT
#include "Header/SwitcherDevice/FiveSixSwicher.h"
// GPIO中断 ---------
FiveSixSwitcher *_sw;
EventGroupHandle_t _handler;
TaskHandle_t _task_handler;
FiveSixSwicherPortConfig _cf = {
    .Up = 10,
    .Down = 6,
    .Left = 2,
    .Right = 3,
    .Mid = 7,
    .Set = 18,
    .Rst = 9,
    .Positive_Enable = true};
void setup()
{
    Serial.begin(115200);

    _sw = new FiveSixSwitcher(_cf);
    _handler = _sw->GetEventHandle();

    auto res = xTaskCreate(KeyProcesser, "Key_Processer_Imp", 512 * 4, nullptr, 0, &_task_handler);
    if (res != pdPASS)
    {
        vTaskDelete(_task_handler);
    }
}
void loop()
{
}

void KeyProcesser(void *data)
{
    auto bits = _sw->GetMaskBits();
    while (true)
    {
        // Serial.printf("等待数据 %x \n", bits);
        auto r = xEventGroupWaitBits(_handler, bits, pdTRUE, pdFALSE, 10); // portTICK_PERIOD_MS
        // Serial.printf("获取到数据: %x\n", r);
        if (r <= 0)
        {
            vTaskDelay(500);
            continue;
        }
        if ((r & (1 << _cf.Up)) == (1 << _cf.Up))
        {
            Serial.println("按钮Up 中断触发");
        }
        else if ((r & (1 << _cf.Down)) == (1 << _cf.Down))
        {
            Serial.println("按钮Down 中断触发");
        }
        else if ((r & (1 << _cf.Left)) == (1 << _cf.Left))
        {
            Serial.println("按钮Left 中断触发");
        }
        else if ((r & (1 << _cf.Right)) == (1 << _cf.Right))
        {
            Serial.println("按钮Right 中断触发");
        }
        else if ((r & (1 << _cf.Mid)) == (1 << _cf.Mid))
        {
            Serial.println("按钮Mid 中断触发");
        }
        else if ((r & (1 << _cf.Set)) == (1 << _cf.Set))
        {
            Serial.println("按钮Set 中断触发");
        }
        else if ((r & (1 << _cf.Rst)) == (1 << _cf.Rst))
        {
            Serial.println("按钮Rst 中断触发");
        }
    };
}

#endif

#ifdef _MY_WEBSERVER
#include <esp_task.h>
#include <freertos/task.h>
#include "Header/Service/WifiService.h"
#include "Header/Service/WebService.h"

DefaultWifiService _wifiservice;
WebService _webservice(80);
// #define STACK_SIZE 16
// static uint8_t ucParamaterToPass;
// TaskHandle_t mHandle = nullptr;

void setup()
{
    Serial.begin(115200);
    _wifiservice.Link(WIFI_MODE_APSTA, "MHHWEntry", "476355288");
    Serial.print(_wifiservice.GetAddress());
    _webservice.Begin();
    // delay(500);
    //  xTaskCreate(I2CMeasure, "Measure", STACK_SIZE, &ucParamaterToPass, tskIDLE_PRIORITY, &mHandle);
    //  configASSERT(mHandle);
}
// bool isa = false;

void loop()
{
    _webservice.Loop();
    //  delay(300);
    //  xQueueSend();
    //  xQueueReceive();
    //  vTaskCreateStatic();
    //  vTaskDelay(1000);
    // Serial.println("Looping 。。。");
}
#endif

#ifdef _MY_SD
#include <esp_task.h>
#include <freertos/task.h>
#include "Header/Service/SDCardService.h"
String mountPoint = "/sdcard";
String filePath = "/test.txt";
// byte buffer[32] = {0};

spi_host_device_t Host_Slot = SPI2_HOST;
// gpio_num_t In_Port = GPIO_NUM_10;
// gpio_num_t Out_Port = GPIO_NUM_3;
// gpio_num_t CS_Port = GPIO_NUM_1;
// gpio_num_t SCLK_Port = GPIO_NUM_2;

gpio_num_t In_Port = GPIO_NUM_10;
gpio_num_t Out_Port = GPIO_NUM_3;
gpio_num_t CS_Port = GPIO_NUM_7;
gpio_num_t SCLK_Port = GPIO_NUM_2;
SDCardService *_sd_card;
bool isInited = false;
void setup()
{
    Serial.begin(115200);
    // gpio_pulldown_dis(Out_Port);
    // gpio_pullup_en(Out_Port);
    // pinMode(Out_Port, INPUT_PULLUP);
}
void loop()
{
    if (!isInited)
    {
        try
        {

            _sd_card = new SDCardService(
                mountPoint,
                Host_Slot,
                In_Port,
                Out_Port,
                CS_Port,
                SCLK_Port);
            auto _my_test_file = new FileObject(mountPoint, filePath, FileMode::Open, (int)FileAccess::Read | (int)FileAccess::Write);
            _my_test_file->WriteChar("OKOK,IM OK !!!");
            _my_test_file->Close();
            delete _my_test_file;
            delete _sd_card;
            isInited = true;
        }
        catch (...)
        {
            Serial.print("初始化SD失败");
        }
    }
    delay(900);
}
#endif

#ifdef _MY_AN_SD
#include <SD.h>
bool isInited = false;
void setup()
{
    Serial.begin(115200);
}

void loop()
{
    if (!isInited)
    {
        // SPI.begin(2,10,3,7);
        // SPI.begin(4,5,6,7);
        if (!SD.begin())
        {
            Serial.print("初始化SD失败");
        }
        else
        {
            isInited = true;
            auto total = SD.totalBytes();
            auto size = SD.cardSize();
            auto type = SD.cardType();
            SD.end();
            Serial.printf("Totoal Bytes = %d \n Size = %d \n Type = %d", total, size, type);
        }
    }
    delay(999);
}
#endif

#ifdef _MY_HTTP_SERVER

#include "Header/Service/WifiService.h"
#include "Header/Service/HttpFileService.h"

DefaultWifiService _wifiservice;

HttpFileService *_http_server;
void setup()
{
    Serial.begin(115200);
    _wifiservice.Link(WIFI_MODE_APSTA, "MHHWEntry", "476355288");
    Serial.print(_wifiservice.GetAddress());
    delay(1000);
    _http_server = new HttpFileService();
}
void loop()
{
}
#endif
