#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <functional>

#include "utils/wifi/WifiUtils.h"
#include "utils/http/HttpServer.h"
#include "utils/weather/Weather.h"
#include "utils/json/JsonParser.h"
#include "utils/fileSystem/FileSystem.h"
#include "utils/FileNames.h"

#include "utils/clock/Clock.h"
#include "utils/wifi/NtpTime.h"

#include "screens/MainScreen.h"

#include <TFT_eSPI.h>

void InitWifi();

void GetWeather(void *parameter);
void GetTime(void *parameter);
void TimeTick(void *parameter);
void MutexTask(const SemaphoreHandle_t &mutex, const TickType_t blockTime, const std::function<void()> task);

TFT_eSPI lcd = TFT_eSPI();
#define LCD_WIDTH 160
#define LCD_HEIGHT 128
#define LCD_ROTATE 3

#define WEATHER_CONFIG_CITY F("city")
#define WEATHER_CONFIG_APIKEY F("apiKey")
#define WEATHER_UPDATE_TIME_OK 10 * 60 * 1000
#define WEATHER_UPDATE_TIME_FAIL 1 * 60 * 1000

#define NTP_SERVER_NAME F("2.ru.pool.ntp.org")
#define TIME_UPDATE_TIME_OK 30 * 60 * 1000
#define TIME_UPDATE_TIME_FAIL 1 * 60 * 1000
#define TIME_UPDATE_FAIL_COUNT 10
#define TIME_MUTEX_WAIT 30
#define CLOCK_TICK_TIME_MILLISEC 100
Clock::Clock myClock(3);
SemaphoreHandle_t timeMutex;

#define SCREEN_MUTEX_WAIT 50
Screens::MainScreen *mainScreen;
SemaphoreHandle_t screenMutex;

void setup()
{
    Serial.begin(115200);

    lcd.init();
    lcd.setRotation(LCD_ROTATE);
    lcd.fillScreen(TFT_BLACK);

    mainScreen = new Screens::MainScreen(&lcd);
    mainScreen->SetVisible(true);

    InitWifi();
    HttpServer::Init(nullptr);

    timeMutex = xSemaphoreCreateMutex();
    screenMutex = xSemaphoreCreateMutex();

    xTaskCreate(
        GetWeather,
        String(F("update weather")).c_str(),
        5 * 1024,
        NULL,
        2,
        NULL);

    xTaskCreate(
        GetTime,
        String(F("update time")).c_str(),
        2 * 1024,
        NULL,
        2,
        NULL);

    xTaskCreate(
        TimeTick,
        String(F("set time")).c_str(),
        2 * 1024,
        NULL,
        3,
        NULL);
}

void loop()
{
    HttpServer::HandleServer();
}

void GetWeather(void *parameter)
{
    for (;;)
    {
        String json = FileSystem::ReadFile(WEATHER_CONFIG_PATH);
        auto weatherCity = JsonParser::GetJsonData(json, WEATHER_CONFIG_CITY);
        auto weatherApiKey = JsonParser::GetJsonData(json, WEATHER_CONFIG_APIKEY);

        bool isOk;
        auto weather = Weather::GetWether(isOk, weatherCity, weatherApiKey);

        MutexTask(screenMutex, SCREEN_MUTEX_WAIT, [weather, isOk]()
                  { mainScreen->SetWeather(weather); });

        if (isOk == true)
        {
            vTaskDelay(WEATHER_UPDATE_TIME_OK / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelay(WEATHER_UPDATE_TIME_FAIL / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
}

void GetTime(void *parameter)
{
    int failCount = 0;

    for (;;)
    {
        bool isOk;
        auto ntp = NtpTime::AskNTP(NTP_SERVER_NAME, isOk);

        if (isOk == true)
        {
            MutexTask(timeMutex, TIME_MUTEX_WAIT, [ntp]()
                      { myClock.ParseFromNtp(ntp); });

            if (failCount != 0)
            {
                MutexTask(screenMutex, SCREEN_MUTEX_WAIT, []()
                          { mainScreen->SetTimeOk(true); });
            }
            failCount = 0;

            vTaskDelay(TIME_UPDATE_TIME_OK / portTICK_PERIOD_MS);
        }
        else
        {
            failCount++;
            if (failCount > TIME_UPDATE_FAIL_COUNT)
            {
                MutexTask(screenMutex, SCREEN_MUTEX_WAIT, []()
                          { mainScreen->SetTimeOk(false); });
            }
            vTaskDelay(TIME_UPDATE_TIME_FAIL / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
}

void TimeTick(void *parameter)
{
    unsigned long prevMillis = millis();

    for (;;)
    {
        MutexTask(timeMutex, TIME_MUTEX_WAIT, [&prevMillis]() mutable
                  {
                      auto nowMillis = millis();
                      myClock.AddMillis(nowMillis - prevMillis);
                      prevMillis = nowMillis;

                      MutexTask(screenMutex, SCREEN_MUTEX_WAIT, []()
                                { mainScreen->SetTime(myClock); });
                  });

        vTaskDelay(CLOCK_TICK_TIME_MILLISEC / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void InitWifi()
{
    int connectTryes = 20;
    auto wifiConfig = WifiUtils::LoadWiFiConfig();

    WifiUtils::TryConnectCallback callback = [wifiConfig, connectTryes](int tryCount)
    {
        String msg = F("try start wifi ");
        msg += String(connectTryes - tryCount);
        msg += '/';
        msg += String(connectTryes);

        mainScreen->SetMessage(msg);
    };

    if (WifiUtils::ConnectWifi(wifiConfig.ssid, wifiConfig.password, connectTryes, callback) == false)
    {
        mainScreen->SetMessage(F("can't connect. start ap"));
        delay(1000);

        String ssid = BASE_SSID;
        String pass = BASE_PASS;

        mainScreen->SetMessage(ssid + ' ' + pass);
        WifiUtils::StartAP(ssid, pass);
        delay(1000);
    }

    mainScreen->SetMessage(String(F("IP: ")) + WifiUtils::GetIpString());
}

void MutexTask(const SemaphoreHandle_t &mutex, const TickType_t blockTime, std::function<void()> task)
{
    for (;;)
    {
        if (xSemaphoreTake(mutex, blockTime) == pdTRUE)
        {
            task();
            xSemaphoreGive(mutex);
            break;
        }
    }
}
