#include "Weather.h"

#include "utils/json/Json.h"

#define WEATHER_HOST F("api.openweathermap.org")
#define CONNECT_WAIT_TIME 3000

namespace Weather
{
    bool ParseCurrentWeather(CurrentWeaterData &currentWeather, const Json &json);
    bool ParseDailyWeather(DailyWeatherData &dailyWeather, const Json &json);

    bool GetCityCoordinates(CityCoordinates &coordinates, const String &city, const String &apiKey)
    {
        String host = String(WEATHER_HOST);
        WiFiClient client;

        if (client.connect(host.c_str(), 80) == 0)
        {
            return false;
        }

        client.print(F("GET /data/2.5/weather?appid="));
        client.print(apiKey);
        client.print(F("&q="));
        client.print(city);

        client.println(F(" HTTP/1.1"));
        client.print(F("Host: "));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        delay(CONNECT_WAIT_TIME);

        String jsonString;
        bool needAdd = false;
        while (client.available())
        {
            char ch = (char)client.read();

            if (ch == '{')
            {
                needAdd = true;
            }

            if (needAdd)
            {
                jsonString += ch;
            }
        }
        client.stop();

        Json json(jsonString);

        bool isOk = json.ContainsName(F("coord"));
        if (isOk == false)
        {
            return false;
        }

        json = json[F("coord")];

        isOk = json.ContainsName(F("lon")) && json.ContainsName(F("lat"));
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

        String host = String(WEATHER_HOST);
        WiFiClient client;

        if (client.connect(host.c_str(), 80) == 0)
        {
            return false;
        }

        client.print(F("GET /data/2.5/onecall?exclude=minutely,hourly,alerts"));
        client.print(F("&appid="));
        client.print(apiKey);
        client.print(F("&lat="));
        client.print(cityCoordinates.lat);
        client.print(F("&lon="));
        client.print(cityCoordinates.lon);

        client.println(F(" HTTP/1.1"));
        client.print(F("Host: "));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        delay(CONNECT_WAIT_TIME);

        String jsonString;
        bool needAdd = false;
        while (client.available())
        {
            char ch = (char)client.read();

            if (ch == '{')
            {
                needAdd = true;
            }

            if (needAdd)
            {
                jsonString += ch;
            }
        }
        client.stop();

        Json json(jsonString);

        if ((json.ContainsName(F("current")) || json.ContainsName(F("daily"))) == false)
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