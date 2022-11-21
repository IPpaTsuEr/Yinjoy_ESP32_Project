#pragma once
#include <hal/ledc_types.h>
#include <driver/ledc.h>
class PWMDevice
{
    ledc_timer_config_t time_config;
    ledc_channel_config_t channel_config;

public:
    PWMDevice()
    {
        time_config.speed_mode = ledc_mode_t::LEDC_LOW_SPEED_MODE;
        time_config.duty_resolution = ledc_timer_bit_t::LEDC_TIMER_13_BIT;
        time_config.timer_num = ledc_timer_t::LEDC_TIMER_0;
        time_config.freq_hz = 5000;
        time_config.clk_cfg = ledc_clk_cfg_t::LEDC_AUTO_CLK;
        ledc_timer_config(&time_config);

        channel_config.gpio_num = 0;
        channel_config.speed_mode = ledc_mode_t::LEDC_LOW_SPEED_MODE;
        channel_config.channel = ledc_channel_t::LEDC_CHANNEL_0;
        channel_config.timer_sel = ledc_timer_t::LEDC_TIMER_0;
        channel_config.intr_type = ledc_intr_type_t::LEDC_INTR_DISABLE;
        channel_config.duty = 0;
        channel_config.hpoint = 0;
        ledc_channel_config(&channel_config);
        
        ledc_fade_func_install(0);
    };

    void HardwareStart(int target, int max_time = 1000)
    {
        ledc_set_fade_with_time(time_config.speed_mode, channel_config.channel, target, max_time);
        ledc_fade_start(time_config.speed_mode, channel_config.channel, LEDC_FADE_NO_WAIT);
    };
    void HardwareStop()
    {
        ledc_fade_func_uninstall();
    };
};