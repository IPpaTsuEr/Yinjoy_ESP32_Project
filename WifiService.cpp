#include "Header/Service/WifiService.h"

void DefaultWifiService::Link(wifi_mode_t Mode, String EntryName = "", String Password = "")
{
    WiFi.mode(Mode);
    if (EntryName != "" && Password != "")
    {
        Serial.printf("Try Linke %s", EntryName.c_str());
        WiFi.begin(EntryName.c_str(), Password.c_str());
    }
    else
    {
        Serial.println("Without Info To Link");
        WiFi.begin();
    }
    while (GetStatus() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Wait WIFI Link...");
    }
};

wl_status_t DefaultWifiService::GetStatus()
{
    return WiFi.status();
};

IPAddress DefaultWifiService::GetAddress()
{
    return WiFi.localIP();
};

std::list<WifiHotPontInformation> *DefaultWifiService::Scan()
{
    // if (WiFi.status() != WL_CONNECTED)
    // {
    //     WiFi.mode(WIFI_MODE_APSTA);
    // }
    auto result = new std::list<WifiHotPontInformation>();
    auto count = WiFi.scanNetworks();
    for (size_t i = 0; i < count; i++)
    {
        result->emplace_back(
            WifiHotPontInformation{
                .SSID = WiFi.SSID(i),
                .Channel = WiFi.channel(i),
                .BSSIDStr = WiFi.BSSIDstr(i),
                .BSSID = WiFi.BSSID(i)});
    }
    return result;
};
