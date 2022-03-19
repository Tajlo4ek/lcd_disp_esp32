#include "Configs.h"

namespace Configs
{

#define CHECK_NAMES(json, ...)                    \
    {                                             \
        for (const auto &name : {__VA_ARGS__})    \
        {                                         \
            if (json.ContainsName(name) == false) \
            {                                     \
                return false;                     \
            }                                     \
        }                                         \
    }

#define TIME_CONFIG_NTP F("ntp")
#define TIME_CONFIG_UTC F("utc")
    bool ConfigFromJson(const Json &json, NtpConfig &config)
    {
        CHECK_NAMES(json, TIME_CONFIG_NTP, TIME_CONFIG_UTC);

        config.ntpServer = json[TIME_CONFIG_NTP].ToString();
        config.utc = json[TIME_CONFIG_UTC].ToInt();

        return true;
    }

#define WEATHER_CONFIG_CITY F("city")
#define WEATHER_CONFIG_APIKEY F("apiKey")
    bool ConfigFromJson(const Json &json, WeatherConfig &config)
    {
        CHECK_NAMES(json, WEATHER_CONFIG_CITY, WEATHER_CONFIG_APIKEY);

        config.city = json[WEATHER_CONFIG_CITY].ToString();
        config.apiKey = json[WEATHER_CONFIG_APIKEY].ToString();

        return true;
    }

}
