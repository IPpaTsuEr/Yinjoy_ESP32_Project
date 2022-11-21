#pragma once
#include "HttpService.h"

class HttpFileService
{
    HttpService _service;

    httpd_uri_t _upload = {
        .uri = "/uploade",
        .method = http_method::HTTP_POST,
        .handler = Upload_handler,
        .user_ctx = nullptr};

    httpd_uri_t _download = {
        .uri = "/download",
        .method = http_method::HTTP_GET,
        .handler = Download_handler,
        .user_ctx = nullptr};

    httpd_uri_t _list = {
        .uri = "/list",
        .method = http_method::HTTP_GET,
        .handler = List_handler,
        .user_ctx = nullptr};

    httpd_uri_t _default = {
        .uri = "/*",
        .method = http_method::HTTP_GET,
        .handler = Default_handler,
        .user_ctx = nullptr};

    static void free_context(void *ctx)
    {
        if (ctx != nullptr)
            free(ctx);
    };

public:
    HttpFileService()
    {
        if (_service.Start())
        {
            _service.RegisterError(httpd_err_code_t::HTTPD_404_NOT_FOUND,&HttpFileService::http_404_handler);
            _service.RegisterError(httpd_err_code_t::HTTPD_500_INTERNAL_SERVER_ERROR, &HttpFileService::http_500_handler);
            _service.RegisterError(httpd_err_code_t::HTTPD_400_BAD_REQUEST, &HttpFileService::http_400_handler);
            _service.Register(&_upload);
            _service.Register(&_download);
            _service.Register(&_list);
            _service.Register(&_default);
        }else{
            Serial.println("启动http服务失败");
        }
    };
    ~HttpFileService(){
        _service.Stop();
    };
    static esp_err_t Default_handler(httpd_req_t *request)
    {
        Serial.println("启动 Default_handler 服务");
        String data = "Ok";
        return httpd_resp_send(request, data.c_str(), data.length());
    };
    static esp_err_t List_handler(httpd_req_t *request)
    {
        Serial.println("启动 List_handler 服务");
        if (!request->sess_ctx)
        {
            // request->sess_ctx = malloc(sizeof(char));
            // request->free_ctx = &HttpFileService::free_context;
        }
        //获取访问的目录

        //获取目录信息

        String data = "";
        return httpd_resp_send(request, data.c_str(), data.length());
    };

    static esp_err_t Download_handler(httpd_req_t *request)
    {
        Serial.println("启动 Download_handler 服务");
        char *buffer = new char[256];
        auto data_size = 256;
        //打开文件

        //待发送文件数据大小
        auto total_size = 0;
        while (total_size > 0)
        {
            //文件数据填充Buffer
            // data_size =
            //发送送数据
            if (ESP_OK == httpd_resp_send_chunk(request, buffer, MIN(total_size, data_size)))
            {
                total_size -= data_size;
            }
            else
            {
                delete[] buffer;
                //关闭文件

                return ESP_FAIL;
            }
        }
        delete[] buffer;
        //关闭文件

        return ESP_OK;
    };

    static esp_err_t Upload_handler(httpd_req_t *request)
    {
        Serial.println("启动 Upload_handler 服务");
        auto session = request->sess_ctx;
        auto notreaded_size = request->content_len;
        char *buffer = new char[256];
        while (notreaded_size > 0)
        {
            auto size = httpd_req_recv(request, buffer, MIN(notreaded_size, 256));
            if (size <= 0)
            {
                if (size == HTTPD_SOCK_ERR_TIMEOUT)
                {
                    continue;
                }
                //处理未完成的文件(删除？)

                delete[] buffer;
                return ESP_FAIL;
            }
            notreaded_size -= size;
            //写入文件流
        }
        //保存关闭文件

        delete[] buffer;
        return ESP_OK;
    };

    static esp_err_t http_404_handler(httpd_req_t *req, httpd_err_code_t error)
    {

        return ESP_OK;
    };

    static esp_err_t http_500_handler(httpd_req_t *req, httpd_err_code_t error)
    {

        return ESP_OK;
    };

    static esp_err_t http_400_handler(httpd_req_t *req, httpd_err_code_t error)
    {

        return ESP_OK;
    };
};