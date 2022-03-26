#include <Arduino.h>
#include <TFT_eSPI.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <queue>
#include <map>

#include "utils/wifi/WifiUtils.h"
#include "utils/http/HttpServer.h"
#include "utils/weather/Weather.h"
#include "utils/json/Json.h"
#include "Configs/Configs.h"
#include "utils/fileSystem/FileSystem.h"

#include "utils/FileNames.h"
#include "utils/Commands.h"
#include "utils/MutexUtils.h"

#include "utils/clock/Clock.h"
#include "utils/wifi/NtpTime.h"

#include "utils/Touch/Touch.h"

#include "screens/MainScreen.h"
#include "screens/VisualizerScreen.h"

void InitWifi();

void GetWeatherTask(void *parameter);
void GetTimeTask(void *parameter);
void ClockTickTime(void *parameter);
void UartTask(void *parameter);
void ParseCommandTask(void *parameter);
void CheckButtonsTask(void *parameter);
void ServerTask(void *parameter);
void TouchTask(void *parameter);

void CreateButtons();
String CheckCommand(const String &data);
void SetActiveScreen(int screenNum);

TFT_eSPI lcd = TFT_eSPI();
#define LCD_ROTATE 1

#define WEATHER_UPDATE_TIME_OK 10 * 60 * 1000
#define WEATHER_UPDATE_TIME_FAIL 1 * 60 * 1000

#define TIME_UPDATE_TIME_OK 30 * 60 * 1000
#define TIME_UPDATE_TIME_FAIL 1 * 60 * 1000
#define CLOCK_TICK_TIME_MILLISEC 100
Clock::Clock myClock;

SemaphoreHandle_t screenMutex;
SemaphoreHandle_t timeMutex;
SemaphoreHandle_t uartMutex;
SemaphoreHandle_t commandQueueMutex;

std::queue<String> commands;

Screens::MainScreen *mainScreen;
Screens::VisualizerScreen *visualizerScreen;

std::vector<Screens::Screen *> screens;
Screens::Screen *activeScreen;
int nowScreenNum;

void setup()
{
    timeMutex = xSemaphoreCreateMutex();
    screenMutex = xSemaphoreCreateMutex();
    commandQueueMutex = xSemaphoreCreateMutex();

    Serial.begin(115200);
    FileSystem::Init();

    lcd.init();
    lcd.setRotation(LCD_ROTATE);
    lcd.fillScreen(TFT_BLACK);

    int width = lcd.width();
    int height = lcd.height();

    mainScreen = new Screens::MainScreen(&lcd, width, height);
    visualizerScreen = new Screens::VisualizerScreen(&lcd, width, height);

    screens.push_back(mainScreen);
    screens.push_back(visualizerScreen);
    SetActiveScreen(0);

    InitWifi();
    HttpServer::Init(CheckCommand);

    xTaskCreate(GetWeatherTask, String(F("update weather")).c_str(), 20 * 1024, NULL, 3, NULL);
    xTaskCreate(GetTimeTask, String(F("update time")).c_str(), 5 * 1024, NULL, 3, NULL);
    xTaskCreate(ClockTickTime, String(F("set time")).c_str(), 5 * 1024, NULL, 4, NULL);
    xTaskCreate(UartTask, String(F("uart rec")).c_str(), 5 * 1024, NULL, 10, NULL);
    xTaskCreate(ParseCommandTask, String(F("command")).c_str(), 10 * 1024, NULL, 10, NULL);
    xTaskCreate(ServerTask, String(F("server")).c_str(), 20 * 1024, NULL, 3, NULL);
    xTaskCreate(TouchTask, String(F("touch")).c_str(), 2 * 1024, NULL, 5, NULL);
}

void loop()
{
    vTaskDelete(NULL);
}

String CheckCommand(const String &data)
{
    String res = COMMAND_FAIL;

    if (data[data.length() - 1] != COMMAND_STOP_CHAR)
    {
        return res;
    }

    if (data.startsWith(COMMAND_SET_MODE_SPECTRUM) ||
        data.startsWith(COMMAND_SEND_SPECTRUM_DATA))
    {
        MUTEX_TASK(screenMutex,
                   {
                       res = visualizerScreen->ParseMessage(data);
                   });
    }
    else if (data.startsWith(COMMAND_RELOAD_SCREEN))
    {
        MUTEX_TASK(screenMutex,
                   {
                       for (const auto &screen : screens)
                       {
                           screen->ReloadConfig();
                       }
                       res = COMMAND_OK;
                   });
    }

    return res;
}

void SetActiveScreen(int screenNum)
{
    MUTEX_TASK(screenMutex,
               {
                   if (screenNum >= (int)screens.size())
                   {
                       screenNum -= screens.size();
                   }
                   else if (screenNum < 0)
                   {
                       screenNum += screens.size();
                   }
                   nowScreenNum = screenNum;

                   for (auto &screen : screens)
                   {
                       screen->SetVisible(false);
                   }

                   activeScreen = screens[nowScreenNum];
                   activeScreen->SetVisible(true);
               });
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

void ServerTask(void *parameter)
{
    for (;;)
    {
        HttpServer::HandleServer();
        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
}

void UartTask(void *parameter)
{
    String serialData;

    for (;;)
    {
        while (Serial.available())
        {
            char ch = (char)Serial.read();
            serialData += ch;

            if (ch == COMMAND_STOP_CHAR)
            {
                MUTEX_TASK(commandQueueMutex,
                           {
                               commands.push(serialData);
                           });

                serialData.clear();
            }
        }

        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void ParseCommandTask(void *parameter)
{
    for (;;)
    {
        String data;

        MUTEX_TASK(commandQueueMutex,
                   {
                       if (commands.empty() == false)
                       {
                           data = commands.front();
                           commands.pop();
                       }
                   });

        if (data.isEmpty() == false)
        {
            Serial.print(CheckCommand(data));
            Serial.print(COMMAND_STOP_CHAR);
        }

        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void GetWeatherTask(void *parameter)
{
    String lastCity;
    Weather::CityCoordinates coordinates;
    bool isCoorOk = false;

    bool isFirts = true;
    byte failCount = 0;

    Configs::WeatherConfig config;

    for (;;)
    {
        Json json(FileSystem::ReadFile(WEATHER_CONFIG_PATH));
        bool isConfigOk = Configs::ConfigFromJson(json, config);

        bool isWeatherOk = false;
        Weather::CurrentWeaterData currentWeather;
        std::vector<Weather::DailyWeatherData> dailyWeather;

        if (isConfigOk)
        {
            if (lastCity != config.city || isCoorOk == false)
            {
                isCoorOk = Weather::GetCityCoordinates(coordinates, config.city, config.apiKey);
                lastCity = config.city;
            }

            if (isCoorOk)
            {
                isWeatherOk = Weather::GetWeather(currentWeather, dailyWeather, coordinates, config.apiKey);
            }
            else
            {
                currentWeather.description = F("can't get data");
                currentWeather.imageName = F("abort");
            }
        }
        else
        {
            currentWeather.description = F("fail load config");
            currentWeather.imageName = F("abort");
        }

        if (isWeatherOk || isFirts || failCount > 2)
        {
            MUTEX_TASK(screenMutex,
                       {
                           mainScreen->SetWeather(currentWeather);
                       });
        }

        if (isWeatherOk == true)
        {
            vTaskDelay(WEATHER_UPDATE_TIME_OK / portTICK_PERIOD_MS);
            failCount = 0;
        }
        else
        {
            vTaskDelay(WEATHER_UPDATE_TIME_FAIL / portTICK_PERIOD_MS);
            failCount++;
        }

        isFirts = false;
    }
}

void GetTimeTask(void *parameter)
{
    byte failCount = 0;
    bool firstLoad = true;

    Configs::NtpConfig config;

    for (;;)
    {
        Json json(FileSystem::ReadFile(TIME_CONFIG_PATH));
        bool isConfigOk = Configs::ConfigFromJson(json, config);

        bool isTimeOk;
        auto time = NtpTime::AskNTP(config.ntpServer, isTimeOk);

        if (isTimeOk && isConfigOk)
        {
            MUTEX_TASK(timeMutex,
                       {
                           myClock.ParseFromNtp(time, config.utc);
                       });

            MUTEX_TASK(screenMutex,
                       {
                           if (failCount != 0 || firstLoad)
                           {
                               mainScreen->SetTimeOk(true);
                           }
                       });

            failCount = 0;

            vTaskDelay(TIME_UPDATE_TIME_OK / portTICK_PERIOD_MS);
        }
        else
        {
            failCount++;
            if (failCount > 5 || firstLoad)
            {
                MUTEX_TASK(screenMutex,
                           {
                               mainScreen->SetTimeOk(false);
                           });
            }
            vTaskDelay(TIME_UPDATE_TIME_FAIL / portTICK_PERIOD_MS);
        }

        firstLoad = false;
    }
}

void ClockTickTime(void *parameter)
{
    unsigned long prevMillis = millis();

    for (;;)
    {
        MUTEX_TASK(timeMutex,
                   {
                       auto nowMillis = millis();
                       myClock.AddMillis(nowMillis - prevMillis);
                       prevMillis = nowMillis;

                       MUTEX_TASK(screenMutex,
                                  {
                                      mainScreen->SetTime(myClock.GetTime(), myClock.GetDate());
                                  });
                   });

        vTaskDelay(CLOCK_TICK_TIME_MILLISEC / portTICK_PERIOD_MS);
    }
}

void TouchTask(void *parameter)
{
    Touch::TouchScreen touchScreen(&lcd);
    Touch::Point touchPoint;

    MUTEX_TASK(screenMutex, {
        touchScreen.Calibrate();
    });
    SetActiveScreen(nowScreenNum);

    while (true)
    {
        MUTEX_TASK(screenMutex, {
            auto isTouched = touchScreen.GetTouch(touchPoint);
            if (isTouched)
            {
                lcd.resetViewport();
                lcd.drawPixel(touchPoint.x, touchPoint.y, TFT_RED);
            }
        });

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
