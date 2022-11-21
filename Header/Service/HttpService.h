#pragma once
#include <esp_http_server.h>

class HttpService
{
    httpd_handle_t _handler = nullptr;

public:
    httpd_config_t _config = HTTPD_DEFAULT_CONFIG();

public:
    HttpService()
    {
        //允许*号匹配
        _config.uri_match_fn = httpd_uri_match_wildcard;
    };
    virtual ~HttpService(){

    };
    bool Start()
    {
        return httpd_start(&_handler, &_config) == ESP_OK;
    };
    bool Stop()
    {
        return httpd_stop(_handler) == ESP_OK;
    };
    bool Register(httpd_uri_t *proc)
    {
        return ESP_OK == httpd_register_uri_handler(_handler, proc);
    };

    bool RegisterError(httpd_err_code_t code, httpd_err_handler_func_t func)
    {
        return ESP_OK == httpd_register_err_handler(_handler, code, func);
    };
};