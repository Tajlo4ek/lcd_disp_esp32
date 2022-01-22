#ifndef _WEATHER_H_
#define _WEATHER_H_

#include <Arduino.h>
#include <WiFiClient.h>

namespace Weather
{
    struct WeatherData
    {
        int temp;
        String description;
        String imageName;
    };

    WeatherData GetWether(bool &isOk, const String &city, const String &apiKey);
}

#endif