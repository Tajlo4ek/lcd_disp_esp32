#include "WifiUtils.h"

#include "utils/FileNames.h"
#include "utils/fileSystem/FileSystem.h"
#include "utils/json/JsonParser.h"

#define SSID_KEY F("ssid")
#define PASSWORD_KEY F("pass")

#define DEFAULT_SSID F("no_data")
#define DEFAULT_PASSWORD F("password")

namespace WifiUtils
{
    bool ConnectWifi(const String &ssid, const String &password, uint connectTries, TryConnectCallback callback = nullptr)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
        while (--connectTries && WiFi.status() != WL_CONNECTED)
        {
            if (callback != nullptr)
            {
                callback(connectTries);
            }
            delay(1000);
        }
        return WiFi.status() == WL_CONNECTED;
    }

    void StartAP(const String &ssid, const String &password)
    {
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        IPAddress apIP;
        apIP.fromString(BASE_IP_STRING);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid.c_str(), password.c_str());
    }

    String GetIpString()
    {
        auto wifiMode = WiFi.getMode();

        if (wifiMode == WIFI_STA)
        {
            return WiFi.localIP().toString();
        }
        else if (wifiMode == WIFI_AP)
        {
            return BASE_IP_STRING;
        }
        else
        {
            return F("error");
        }
    }

    WiFiConfig LoadWiFiConfig()
    {
        WiFiConfig config;
        config.ssid = DEFAULT_SSID;
        config.password = DEFAULT_PASSWORD;

        if (FileSystem::FileExists(WIFI_CONFIG_PATH) == false)
        {
            SaveWiFiConfig(config);
            return config;
        }

        String json = FileSystem::ReadFile(WIFI_CONFIG_PATH);

        bool isOk = false;
        auto ssid = JsonParser::GetJsonData(json, SSID_KEY, isOk);
        if (isOk == false)
        {
            return config;
        }

        auto password = JsonParser::GetJsonData(json, PASSWORD_KEY, isOk);
        if (isOk == false)
        {
            return config;
        }

        config.ssid = ssid;
        config.password = password;
        return config;
    }

    void SaveWiFiConfig(WiFiConfig config)
    {
        const int dataCount = 2;
        String names[dataCount]{SSID_KEY, PASSWORD_KEY};
        String data[dataCount]{config.ssid, config.password};
        String json = JsonParser::BuildJson(names, data, dataCount);
        FileSystem::WriteFile(WIFI_CONFIG_PATH, json);
    }

    std::vector<String> GetSSIDs()
    {
        std::vector<String> ssids;
        auto wifiCount = WiFi.scanNetworks();
        for (int i = 0; i < wifiCount; i++)
        {
            auto ssid = WiFi.SSID(i);
            if (ssid.isEmpty() == false)
            {
                ssids.push_back(ssid);
            }
        }
        return ssids;
    }

} // namespace WifiUtils