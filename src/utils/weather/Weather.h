#ifndef _WEATHER_H_
#define _WEATHER_H_

#include <Arduino.h>
#include <WiFiClient.h>

namespace Weather
{
    struct CurrentWeaterData
    {
        int temp;
        int pressure;
        int humidity;
        String description;
        String imageName;
    };

    struct DailyWeatherData : CurrentWeaterData
    {
        unsigned long time;
        int tempDay;
        int tempNight;
    };

    struct CityCoordinates
    {
        float lat;
        float lon;
    };

    bool GetCityCoordinates(CityCoordinates &coordinates, const String &city, const String &apiKey);
    bool GetWeather(CurrentWeaterData &currentWeather,
                    std::vector<DailyWeatherData> &dailyWeather,
                    const CityCoordinates &cityCoordinates,
                    const String &apiKey);
}

#endif