#pragma once

#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_hidd_api.h>
#include <esp_gap_bt_api.h>
#include <esp_gatts_api.h>

class BlueHIDDevice
{
    esp_bt_controller_config_t bt_controller = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    string bt_device_name = "YinJoy Mouse";
    esp_bt_cod_t _code;
    esp_bt_pin_type_t _pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t _pin_code;

    esp_hidd_app_param_t app_param;
    esp_hidd_qos_param_t both_qos;

    uint8_t _current_mode;
    uint8_t _buffer[4];

#define BOOT_PROTO_MOUSE_RPT_ID 0x02
protected:
    const EventGroupHandle_t Hidd_event_handle;
    const uint8_t HIDD_OPEND_BIT = 0b01;
    const uint8_t HIDD_CLOSED_BIT = 0b10;

public:
    BlueHIDDevice()
    {
        esp_bt_controller_init(&bt_controller);
        esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
        esp_bluedroid_init();
        esp_bluedroid_enable();
        esp_bt_gap_register_callback(GAP_Event_Callback);

        esp_bt_dev_set_device_name(bt_device_name.c_str());
        _code.major = ESP_BT_COD_MAJOR_DEV_PERIPHERAL;
        esp_bt_gap_set_cod(_code, ESP_BT_SET_COD_MAJOR_MINOR);

        delay(1000);

        esp_bt_hid_device_register_callback(HID_Event_Callback);
        esp_bt_hid_device_init();

        esp_bt_gap_set_pin(_pin_type, 0, _pin_code);

        Hidd_event_handle = xEventGroupCreate();
    };
    ~BlueHIDDevice()
    {
        if (Hidd_event_handle != nullptr)
        {
            vEventGroupDelete(Hidd_event_handle);
        }
    };

    void OnHiddOpend()
    {
        auto value = xEventGroupSetBits(Hidd_event_handle, HIDD_OPEND_BIT);
        // xTaskCreate(func, "OnHiddopend", 1024, nullptr, 1, nullptr);
    }
    void OnHiddClosed()
    {
        auto value = xEventGroupSetBits(Hidd_event_handle, HIDD_CLOSED_BIT);
    }

    void GAP_Event_Callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
    {
        switch (event)
        {
        case ESP_BT_GAP_DISC_RES_EVT:
            break; /*!< Device discovery result event */
        case ESP_BT_GAP_DISC_STATE_CHANGED_EVT:
            break; /*!< Discovery state changed event */
        case ESP_BT_GAP_RMT_SRVCS_EVT:
            break; /*!< Get remote services event */
        case ESP_BT_GAP_RMT_SRVC_REC_EVT:
            break; /*!< Get remote service record event */
        case ESP_BT_GAP_AUTH_CMPL_EVT:
            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
            {
                Serial.Println("认证成功");
            }
            else
            {
                Serial.printf("认证失败:%d \n", param->auth_cmpl.stat);
            }
            break; /*!< Authentication complete event */
        case ESP_BT_GAP_PIN_REQ_EVT:
            if (param->pin_req.min_16_digit)
            {
                Serial.println("Pin 代码 : 0000 0000 0000 0000");
                esp_bt_code_t pincode = {0};
                esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
            }
            else
            {
                Serial.println("Pin 代码 : 1 2 3 4");
                esp_bt_code_t pincode = {0};
                pin_code[0] = '1';
                pin_code[1] = '2';
                pin_code[2] = '3';
                pin_code[3] = '4';
                esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
            }
            break; /*!< Legacy Pairing Pin code request */
        case ESP_BT_GAP_CFM_REQ_EVT:
            break; /*!< Security Simple Pairing User Confirmation request. */
        case ESP_BT_GAP_KEY_NOTIF_EVT:
            break; /*!< Security Simple Pairing Passkey Notification */
        case ESP_BT_GAP_KEY_REQ_EVT:
            break; /*!< Security Simple Pairing Passkey request */
        case ESP_BT_GAP_READ_RSSI_DELTA_EVT:
            break; /*!< Read rssi event */
        case ESP_ BT_GAP_CONFIG_EIR_DATA_EVT:
            break; /*!< Config EIR data event */
        case ESP_BT_GAP_SET_AFH_CHANNELS_EVT:
            break; /*!< Set AFH channels event */
        case ESP_BT_GAP_READ_REMOTE_NAME_EVT:
            break; /*!< Read Remote Name event */
        case ESP_BT_GAP_MODE_CHG_EVT:
            Serial.printf("ESP_BT_GAP_MODE_CHG_EVT mode:%d \n", param->mode_chg.mode);
            break;
        case ESP_BT_GAP_REMOVE_BOND_DEV_COMPLETE_EVT:
            break; /*!< remove bond device complete event */
        case ESP_BT_GAP_QOS_CMPL_EVT:
            break; /*!< QOS complete event */
        case ESP_BT_GAP_EVT_MAX:
            break;

        default:
            break;
        }
    };
    void HID_Event_Callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
    {
        switch (event)
        {
        case ESP_HIDD_INIT_EVT:
            if (param->init.status == ESP_HIDD_SUCCESS)
            {
                Serial.println("设置HID参数");
                esp_bt_hid_device_register_app(&app_param, &both_qos, &both_qos);
            }
            else
            {
                Serial.println("HID初始化失败");
            }
            break;

        case ESP_HIDD_DEINIT_EVT:
            Serial.println("HID反初始化");
            break;
        case ESP_HIDD_REGISTER_APP_EVT:
            if (param->register_app.status == ESP_HIDD_SUCCESS)
            {
                Serial.println("设置HID参数成功");
                Serial.println("设置可连接可发现状态");
                esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
                if (param->register_app.in_use && param->register_app.bd_addr != NULL)
                {
                    Serial.println("连接");
                    esp_bt_hid_device_connect(param->register_app.bd_addr);
                }
            }
            else
            {
                serial.println("设置HDI参数失败");
            }
            break;
        case ESP_HIDD_UNREGISTER_APP_EVT:
            if (param->unregister_app.status == ESP_HIDD_SUCCESS)
            {
                Serial.println("反注册 APP 成功");
            }
            Serial.println("反注册 APP 失败");
            break;
        case ESP_HIDD_OPEN_EVT:
            if (param->open.status == ESP_HIDD_SUCCESS)
            {
                if (param->open.conn_status == ESP_HIDD_CONN_STATE_CONNECTING)
                {
                    Serial.println("正在连接...");
                }
                else if (param->open.conn_status == ESP_HIDD_CONN_STATE_DISCONNECTED)
                {
                    Serial.Printf("连接成功，%02x:%02x:%02x:%02x:%02x:%02x",
                                  param->open.bd_addr[0],
                                  param->open.bd_addr[1],
                                  param->open.bd_addr[2],
                                  param->open.bd_addr[3],
                                  param->open.bd_addr[4],
                                  param->open.bd_addr[5]);
                    Serial.println("设备已连接，设置为不可发现、不可扫描状态");
                    esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
                    // Raise TASK
                    OnHiddOpend();
                }
                else
                {
                    Serial.println("未知连接状态");
                }
            }
            else
            {
                Serial.println("连接失败");
            }
            break;
        case ESP_HIDD_CLOSE_EVT:
            if (param->close.status == ESP_HIDD_SUCCESS)
            {
                if (param->close.conn_status == ESP_HIDD_CONN_STATE_DISCONNECTING)
                {
                    Serial.println("正在断开连接");
                }
                else if (param->close.conn_status == ESP_HIDD_CONN_STATE_DISCONNECTED)
                {
                    Serial.println("设备已断开连接，重新设置为可发现、可扫描状态");
                    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_DISCOVERABLE);
                    // Stop TASK
                    OnHiddClosed();
                }
                else
                {
                    Serial.println("未知连接状态");
                }
            }
            else
            {
                Serial.println("断开连接失败");
            }
            break;
        case ESP_HIDD_SEND_REPORT_EVT:

            break;
        case ESP_HIDD_REPORT_ERR_EVT:

            break;
        case ESP_HIDD_GET_REPORT_EVT:
            // HID设备接收主机的GET_REPORT请求：ESP_HIDD_GET_REPORT_EVT
            // check_report_id_type(): 确定report_id与report_type
            // esp_bt_hid_device_send_report(): 根据请求的参数返回report。
            auto id = param->get_report.report_id;
            auto type = param->get_report.report_type;
            bool nopassed = false;
            if (type != ESP_HIDD_REPORT_TYPE_INPUT)
            {
            }
            else
            {
                if (_current_mode == ESP_HIDD_BOOT_MODE)
                {
                    if (id == BOOT_REPORT_MOUSE_RPT_ID)
                        nopassed = true;
                }
                else
                {
                    if (id == 0)
                        nopassed = true;
                }
            }
            if (!nopassed)
            {
                esp_bt_hid_device_report_error(ESP_HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID);
            }
            if (nopassed)
            {
                if (ESP_HIDD_REPORT_MODE == _crrent_mode)
                {
                    esp_bt_hid_device_send_report(param->get_report.report_type, 0x0, 4, _buffer);
                }
                else if (ESP_HIDD_BOOT_MODE)
                {
                    esp_bt_hid_device_send_report(param->get_report, 0x02, 3, _buffer);
                }
            }
            break;
        case ESP_HIDD_SET_REPORT_EVT:
            break;

        case ESP_HIDD_SET_PROTOCOL_EVT:
            auto mode = param->set_protocol.protocol_mode;
            if (mode == ESP_HIDD_BOOT_MODE)
            {
            }
            else if (ESP_HIDD_REPORT_MODE == mode)
            {
            }
            _current_mode = mode;
            break;

        case ESP_HIDD_INTR_DATA_EVT:
            esp_bt_hid_device_send_report(ESP_HIDD_REPORT_TYPE_INTRDATA, 0x00, 4, _buffer);
            break;
        case ESP_HIDD_VC_UNPLUG_EVT:
            if (ESP_HIDD_SUCCESS == param->vc_unplug.status)
            {
                if (ESP_HIDD_CONN_STATE_DISCONNECTED)
                {
                    Serial.println("设备连接丢失");
                    esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
                }
            }
            break;

        default:
            break;
        }
    };
}