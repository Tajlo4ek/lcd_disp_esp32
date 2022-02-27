#ifndef _WIFI_UTILS_H_
#define _WIFI_UTILS_H_

#include <Arduino.h>
#include <WiFi.h>

#define BASE_IP_STRING F("192.168.0.43")
#define BASE_SSID F("HOME")
#define BASE_PASS F("1234567890")

namespace WifiUtils
{
    typedef std::function<void(int count)> TryConnectCallback;

    struct WiFiConfig
    {
        String ssid;
        String password;
    };

    bool ConnectWifi(const String &ssid, const String &password, uint connectTries, TryConnectCallback callback);
    void StartAP(const String &ssid, const String &password);

    WiFiConfig LoadWiFiConfig();
    void SaveWiFiConfig(WiFiConfig config);

    String GetIpString();
    std::vector<String> GetSSIDs();

}

#endif
