#include "Weather.h"

#include "utils/json/Json.h"
#include "utils/http/HttpClient.h"

#define WEATHER_HOST F("api.openweathermap.org")

namespace Weather
{
    bool ParseCurrentWeather(CurrentWeaterData &currentWeather, const Json &json);
    bool ParseDailyWeather(DailyWeatherData &dailyWeather, const Json &json);

    bool GetCityCoordinates(CityCoordinates &coordinates, const String &city, const String &apiKey)
    {
        String host(WEATHER_HOST);
        String startLine(F("GET /data/2.5/weather"));
        std::vector<std::pair<String, String>> params{
            {F("appid"), apiKey},
            {F("q"), city},
        };

        int code = -1;
        Json json(HttpClient::GetHttpContent(host, startLine, params, code));

        bool isOk = code == 200;
        isOk &= json.ContainsName(F("coord"));
        json = json[F("coord")];

        isOk &= json.ContainsName(F("lon"));
        isOk &= json.ContainsName(F("lat"));

        if (isOk == false)
        {
            return false;
        }

        coordinates.lon = json[F("lon")].ToFloat();
        coordinates.lat = json[F("lat")].ToFloat();
        return true;
    }

    bool GetWeather(CurrentWeaterData &currentWeather,
                    std::vector<DailyWeatherData> &dailyWeather,
                    const CityCoordinates &cityCoordinates,
                    const String &apiKey)
    {

        String host(WEATHER_HOST);
        String startLine(F("GET /data/2.5/onecall"));
        std::vector<std::pair<String, String>> params{
            {F("exclude"), F("minutely,hourly,alerts")},
            {F("appid"), apiKey},
            {F("lat"), String(cityCoordinates.lat)},
            {F("lon"), String(cityCoordinates.lon)},
        };

        int code = -1;
        Json json(HttpClient::GetHttpContent(host, startLine, params, code));

        bool isOk = code == 200;
        isOk &= json.ContainsName(F("current"));
        isOk &= json.ContainsName(F("daily"));

        if (isOk == false)
        {
            return false;
        }

        ParseCurrentWeather(currentWeather, json[F("current")]);

        json = json[F("daily")];
        dailyWeather.clear();

        for (const auto &dailyJson : json.GetArray())
        {
            DailyWeatherData weather;

            bool isOk = ParseDailyWeather(weather, dailyJson);
            if (isOk == false)
            {
                return false;
            }

            dailyWeather.push_back(weather);
        }

        return true;
    }

    bool ParseCurrentWeather(CurrentWeaterData &currentWeather, const Json &json)
    {
        bool isOk = json.ContainsName(F("temp"));
        isOk &= json.ContainsName(F("pressure"));
        isOk &= json.ContainsName(F("humidity"));
        isOk &= json.ContainsName(F("weather"));

        Json weatherJson = json[F("weather")][0];
        isOk &= weatherJson.ContainsName(F("description"));
        isOk &= weatherJson.ContainsName(F("icon"));

        if (isOk == false)
        {
            return false;
        }

        currentWeather.temp = json[F("temp")].ToInt() - 273;
        currentWeather.humidity = json[F("humidity")].ToInt();
        currentWeather.pressure = json[F("pressure")].ToInt();
        currentWeather.description = weatherJson[F("description")].ToString();
        currentWeather.imageName = weatherJson[F("icon")].ToString();

        return true;
    }

    bool ParseDailyWeather(DailyWeatherData &dailyWeather, const Json &json)
    {
        bool isOk = json.ContainsName(F("dt"));
        isOk &= json.ContainsName(F("pressure"));
        isOk &= json.ContainsName(F("humidity"));

        Json weatherJson = json[F("weather")][0];
        isOk &= weatherJson.ContainsName(F("description"));
        isOk &= weatherJson.ContainsName(F("icon"));

        Json tempJson = json[F("temp")];
        isOk &= tempJson.ContainsName(F("day"));
        isOk &= tempJson.ContainsName(F("night"));
        isOk &= tempJson.ContainsName(F("min"));
        isOk &= tempJson.ContainsName(F("max"));

        if (isOk == false)
        {
            return false;
        }

        dailyWeather.time = json[F("dt")].ToLong();

        dailyWeather.temp = (tempJson[F("min")].ToInt() + tempJson[F("max")].ToInt()) / 2 - 273;
        dailyWeather.tempDay = tempJson[F("day")].ToInt() - 273;
        dailyWeather.tempNight = tempJson[F("night")].ToInt() - 273;

        dailyWeather.humidity = json[F("humidity")].ToInt();
        dailyWeather.pressure = json[F("pressure")].ToInt();

        dailyWeather.description = weatherJson[F("description")].ToString();
        dailyWeather.imageName = weatherJson[F("icon")].ToString();

        return true;
    }
}