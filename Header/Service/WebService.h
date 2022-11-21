#include <WiFi.h>

class WebService
{
    WiFiServer *_server;

public:
    WebService(size_t port)
    {
        _server = new WiFiServer(port);
        Serial.println("WebService Created");
    };
    ~WebService()
    {
        if (_server != nullptr)
        {
            delete _server;
        }
    };
    void Begin()
    {
        _server->begin();
        Serial.println("WebService Started");
    };
    void Loop(String data)
    {
        WiFiClient client = _server->available();
        Serial.println("WebClient Created");
        client.setTimeout(10000);

        if (client)
        {
            Serial.println("WebClient Listening");
            while (client.connected())
            {
                if (client.available())
                {
                    Serial.println("WebClient Linked");

                    while (client.available())
                    {
                        auto data = client.read();
                        Serial.print(data);
                    }
                    Serial.println("Read End");
                    Serial.println(data);
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type:application/json");
                    client.print("\n\n");
                    //client.println("<!DOCTYPE html><html> <head> <meta charset=\"utf8\"> <title>文档标题</title> </head>  <body>");
                    client.println(data);
                    //client.println(" </body> </html>");
                    //client.println();
                    Serial.println("Send End");
                    client.stop();
                    Serial.println("Client Stop ");
                }
            }
        }
        else
        {
            Serial.println("Client Is null");
        }
    }
};
