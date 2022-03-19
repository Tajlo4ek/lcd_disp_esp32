#ifndef _CONFIGS_BASECONFIG_H_
#define _CONFIGS_BASECONFIG_H_

#include "utils/json/Json.h"

namespace Configs
{
    struct NtpConfig
    {
        String ntpServer;
        int utc;
    };

    struct WeatherConfig
    {
        String city;
        String apiKey;
    };

    bool ConfigFromJson(const Json &json, NtpConfig &config);
    bool ConfigFromJson(const Json &json, WeatherConfig &config);
}

#endif