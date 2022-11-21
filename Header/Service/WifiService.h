#include <WiFi.h>
#include <list>

    typedef struct _WifiHotPontInformation
    {
        String SSID;
        int32_t Channel;
        String BSSIDStr;
        uint8_t *BSSID;
    } WifiHotPontInformation;

    class DefaultWifiService
    {
    public:
        DefaultWifiService(){};
        void Link(wifi_mode_t Mode, String EntryName, String Password);

        wl_status_t GetStatus();

        IPAddress GetAddress();

        static std::list<WifiHotPontInformation> *Scan();
    };
