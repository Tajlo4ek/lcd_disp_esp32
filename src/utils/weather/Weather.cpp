#include "Weather.h"

#include "utils/json/JsonParser.h"

namespace Weather
{
    void ParseWeather(const String &json, WeatherData &weather, bool &isOk);
    WeatherData GetWether(bool &isOk, const String &city, const String &apiKey);
    void SetAbortWeather(WeatherData &weather, const String &description);

    WeatherData GetWether(bool &isOk, const String &city, const String &apiKey)
    {
        WeatherData weatherData;

        if (city.isEmpty() || apiKey.isEmpty())
        {
            SetAbortWeather(weatherData, F("check city or apiKey"));
            isOk = false;
            return weatherData;
        }

        String host = String(F("api.openweathermap.org"));
        WiFiClient client;

        if (client.connect(host.c_str(), 80) == 0)
        {
            SetAbortWeather(weatherData, F("cannot connect to server"));
            isOk = false;
            return weatherData;
        }

        client.print(F("GET "));
        client.print(F("/data/2.5/weather?appid="));
        client.print(apiKey);
        client.print(F("&q="));
        client.print(city);

        client.println(F(" HTTP/1.1"));
        client.print(F("Host: "));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        delay(3000);

        String json;
        while (client.available())
        {
            json += (char)client.read();
        }
        json += '}';

        client.stop();
        ParseWeather(json, weatherData, isOk);

        return weatherData;
    }

    void SetAbortWeather(WeatherData &weather, const String &description)
    {
        weather.imageName = F("abort");
        weather.description = description;
        weather.temp = 99;
    }

    void ParseWeather(const String &json, WeatherData &weather, bool &isOk)
    {
        String codString = JsonParser::GetJsonData(json, F("cod"), isOk);

        if (isOk == false)
        {
            SetAbortWeather(weather, F("cannot get gata"));
            return;
        }

        auto cod = codString.toInt();
        if (cod != 200)
        {
            String decription = F("not sync. error: ");
            decription += cod;
            SetAbortWeather(weather, decription);
            isOk = false;
            return;
        }

        bool descIsOk;
        bool imgIsOk;
        bool tempIsOk;
        weather.description = JsonParser::GetJsonData(json, F("description"), descIsOk);
        weather.imageName = JsonParser::GetJsonData(json, F("icon"), imgIsOk);
        weather.temp = JsonParser::GetJsonData(json, F("temp"), tempIsOk).toInt() - 273;

        if ((descIsOk && imgIsOk && tempIsOk) == false)
        {
            SetAbortWeather(weather, F("parse error"));
            isOk = false;
            return;
        }

        auto firstChar = weather.description[0];
        //first char to upper
        if (firstChar >= 'a' && firstChar <= 'z')
        {
            weather.description.setCharAt(0, firstChar - ('a' - 'A'));
        }
        isOk = true;
    }

}