#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <queue>
#include <map>

#include "utils/wifi/WifiUtils.h"
#include "utils/http/HttpServer.h"
#include "utils/weather/Weather.h"
#include "utils/json/JsonParser.h"
#include "utils/fileSystem/FileSystem.h"
#include "utils/button/Button.h"

#include "utils/FileNames.h"
#include "utils/Commands.h"
#include "utils/MutexUtils.h"

#include "utils/clock/Clock.h"
#include "utils/wifi/NtpTime.h"

#include "screens/MainScreen.h"
#include "screens/VisualizerScreen.h"

#include <TFT_eSPI.h>

void InitWifi();

void GetWeatherTask(void *parameter);
void GetTimeTask(void *parameter);
void TimeTickTask(void *parameter);
void UartTask(void *parameter);
void ParseCommandTask(void *parameter);
void CheckButtonsTask(void *parameter);
void ServerTask(void *parameter);

void CreateButtons();
String CheckCommand(const String &data);
void SetActiveScreen(int screenNum);

void IRAM_ATTR BtnISR();

TFT_eSPI lcd = TFT_eSPI();
#define LCD_WIDTH 160
#define LCD_HEIGHT 128
#define LCD_ROTATE 3

bool isSTA;

#define WEATHER_CONFIG_CITY F("city")
#define WEATHER_CONFIG_APIKEY F("apiKey")
#define WEATHER_UPDATE_TIME_OK 10 * 60 * 1000
#define WEATHER_UPDATE_TIME_FAIL 1 * 60 * 1000

#define NTP_SERVER_NAME F("2.ru.pool.ntp.org")
#define TIME_UPDATE_TIME_OK 30 * 60 * 1000
#define TIME_UPDATE_TIME_FAIL 1 * 60 * 1000
#define TIME_UPDATE_FAIL_COUNT 10
#define CLOCK_TICK_TIME_MILLISEC 100
Clock::Clock myClock(3);

SemaphoreHandle_t screenMutex;
SemaphoreHandle_t timeMutex;
SemaphoreHandle_t uartMutex;
SemaphoreHandle_t commandQueueMutex;

std::queue<String> commands;

#define BTN_PIN_OK 27
#define BTN_PIN_LEFT 12
#define BTN_PIN_DOWN 14
#define BTN_PIN_UP 26
#define BTN_PIN_RIGHT 13
QueueHandle_t buttonClickQueue;
std::map<Utils::ButtonName, Utils::Button> buttons;

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

    buttonClickQueue = xQueueCreate(100, sizeof(Utils::ButtonName));

    Serial.begin(115200);
    FileSystem::Init();

    lcd.init();
    lcd.setRotation(LCD_ROTATE);
    lcd.fillScreen(TFT_BLACK);

    mainScreen = new Screens::MainScreen(&lcd);
    visualizerScreen = new Screens::VisualizerScreen(&lcd);

    screens.push_back(mainScreen);
    screens.push_back(visualizerScreen);
    SetActiveScreen(0);

    InitWifi();
    HttpServer::Init(CheckCommand);

    CreateButtons();

    xTaskCreate(GetWeatherTask, String(F("update weather")).c_str(), 5 * 1024, NULL, 3, NULL);
    xTaskCreate(GetTimeTask, String(F("update time")).c_str(), 5 * 1024, NULL, 3, NULL);
    xTaskCreate(TimeTickTask, String(F("set time")).c_str(), 5 * 1024, NULL, 4, NULL);
    xTaskCreate(UartTask, String(F("uart rec")).c_str(), 5 * 1024, NULL, 10, NULL);
    xTaskCreate(ParseCommandTask, String(F("command")).c_str(), 10 * 1024, NULL, 10, NULL);
    xTaskCreate(CheckButtonsTask, String(F("buttons")).c_str(), 10 * 1024, NULL, 10, NULL);
    xTaskCreate(ServerTask, String(F("server")).c_str(), 20 * 1024, NULL, 3, NULL);
}

void loop()
{
    vTaskDelete(NULL);
}

void CreateButtons()
{
#define CREATE_BUTTON(pin, name, func)                          \
    {                                                           \
        buttons[name] = Utils::Button();                        \
        buttons[name].SetClickCallback([]() { func });          \
        pinMode(pin, INPUT);                                    \
        attachInterrupt(                                        \
            pin, []() {                                         \
                Utils::ButtonName value = name;                 \
                xQueueSendFromISR(buttonClickQueue, &value, 0); \
            },                                                  \
            RISING);                                            \
    }

#define CHECK_BTN_CLICK_SCREEN(screenFunc, notScreenFunc) \
    {                                                     \
        bool isInterapt;                                  \
        MutexTask(screenMutex,                            \
                  {                                       \
                      isInterapt = screenFunc;            \
                  });                                     \
        if (isInterapt == false)                          \
        {                                                 \
            notScreenFunc;                                \
        }                                                 \
    }

    CREATE_BUTTON(
        BTN_PIN_RIGHT,
        Utils::ButtonName::NameRight,
        CHECK_BTN_CLICK_SCREEN(
            activeScreen->OnBtnRightClick(),
            SetActiveScreen(nowScreenNum + 1)));

    CREATE_BUTTON(
        BTN_PIN_LEFT,
        Utils::ButtonName::NameLeft,
        CHECK_BTN_CLICK_SCREEN(
            activeScreen->OnBtnLeftClick(),
            SetActiveScreen(nowScreenNum - 1)));

    CREATE_BUTTON(
        BTN_PIN_OK,
        Utils::ButtonName::NameOk,
        {
            //Serial.println("NameOk");
        });

    CREATE_BUTTON(
        BTN_PIN_DOWN,
        Utils::ButtonName::NameDown,
        {
            //Serial.println("NameDown");
        });

    CREATE_BUTTON(
        BTN_PIN_UP,
        Utils::ButtonName::NameUp,
        {
            //Serial.println("NameUp");
        });

#undef CHECK_BTN_CLICK_SCREEN
#undef CREATE_BUTTON
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
        MutexTask(screenMutex,
                  {
                      res = visualizerScreen->ParseMessage(data);
                  });
    }
    else if (data.startsWith(COMMAND_RELOAD_SCREEN))
    {
        MutexTask(screenMutex,
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
    MutexTask(screenMutex,
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
    isSTA = true;
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
        isSTA = false;
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
                MutexTask(commandQueueMutex,
                          {
                              commands.push(serialData);
                          });

                serialData.clear();
            }
        }

        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void CheckButtonsTask(void *parameter)
{
    for (;;)
    {
        Utils::ButtonName btnName;
        xQueueReceive(buttonClickQueue, &btnName, portMAX_DELAY);
        buttons[btnName].CheckClick();
    }
}

void ParseCommandTask(void *parameter)
{
    for (;;)
    {
        String data;

        MutexTask(commandQueueMutex,
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
    for (;;)
    {
        String json = FileSystem::ReadFile(WEATHER_CONFIG_PATH);
        auto weatherCity = JsonParser::GetJsonData(json, WEATHER_CONFIG_CITY);
        auto weatherApiKey = JsonParser::GetJsonData(json, WEATHER_CONFIG_APIKEY);

        bool isOk;
        auto weather = Weather::GetWether(isOk, weatherCity, weatherApiKey);

        MutexTask(screenMutex,
                  {
                      mainScreen->SetWeather(weather);
                  });

        if (isOk == true)
        {
            vTaskDelay(WEATHER_UPDATE_TIME_OK / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelay(WEATHER_UPDATE_TIME_FAIL / portTICK_PERIOD_MS);
        }
    }
}

void GetTimeTask(void *parameter)
{
    int failCount = 0;

    for (;;)
    {
        bool isOk;
        auto ntp = NtpTime::AskNTP(NTP_SERVER_NAME, isOk);

        if (isOk == true)
        {
            MutexTask(timeMutex,
                      {
                          myClock.ParseFromNtp(ntp);
                      });

            if (failCount != 0)
            {
                MutexTask(screenMutex,
                          {
                              mainScreen->SetTimeOk(true);
                          });
            }
            failCount = 0;

            vTaskDelay(TIME_UPDATE_TIME_OK / portTICK_PERIOD_MS);
        }
        else
        {
            failCount++;
            if (failCount > TIME_UPDATE_FAIL_COUNT)
            {
                MutexTask(screenMutex,
                          {
                              mainScreen->SetTimeOk(false);
                          });
            }
            vTaskDelay(TIME_UPDATE_TIME_FAIL / portTICK_PERIOD_MS);
        }
    }
}

void TimeTickTask(void *parameter)
{
    unsigned long prevMillis = millis();

    for (;;)
    {
        MutexTask(timeMutex,
                  {
                      auto nowMillis = millis();
                      myClock.AddMillis(nowMillis - prevMillis);
                      prevMillis = nowMillis;

                      MutexTask(screenMutex,
                                {
                                    mainScreen->SetTime(myClock);
                                });
                  });

        vTaskDelay(CLOCK_TICK_TIME_MILLISEC / portTICK_PERIOD_MS);
    }
}
