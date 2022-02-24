#include "MainScreen.h"

#include "utils/FileNames.h"
#include "utils/Commands.h"
#include "utils/weather/WeatherImages.h"

#include "utils/fileSystem/FileSystem.h"
#include "utils/json/JsonParser.h"
#include "utils/DrawUtils/Color.h"

namespace Screens
{
#define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_CLOCK_MAIN_COLOR F("clockMainColor")
#define CONFIG_CLOCK_SECOND_COLOR F("clockSecondColor")
#define CONFIG_ERROR_COLOR F("errorColor")

    MainScreen::MainScreen(TFT_eSPI *lcd, int width, int height)
        : Screen(lcd, width, height)
    {

        Controls::ControlRect labelMessageRect = {0, 5, width, height / 10};
        this->labelMessage = new Controls::Label(lcd, labelMessageRect);
        SetMessage("");
        controls.push_back(this->labelMessage);

        Controls::ControlRect digitalClockRect = {0, labelMessageRect.leftUpY + labelMessageRect.height + 1, width, height * 10 / 25};
        this->digitalClock = new Controls::DigitalClock(lcd, digitalClockRect);
        controls.push_back(this->digitalClock);

        Controls::ControlRect imageWeatherRect = {5, height - 96, 96, 96};
        this->imageWeather = new Controls::Image(lcd, imageWeatherRect);
        controls.push_back(this->imageWeather);

        Controls::ControlRect labelTempRect = {
            width - imageWeatherRect.height * 3 / 2 - 5,
            height - imageWeatherRect.height,
            imageWeatherRect.height * 3 / 2,
            imageWeatherRect.height};
        this->labelTemp = new Controls::Label(lcd, labelTempRect);
        controls.push_back(this->labelTemp);

        Controls::ControlRect labelWeatherDescriptionRect = {
            imageWeatherRect.leftUpX + imageWeatherRect.width + 5,
            height - imageWeatherRect.height,
            labelTempRect.leftUpX - (imageWeatherRect.leftUpX + imageWeatherRect.width) - 10,
            imageWeatherRect.height};

        this->labelWeatherDescription = new Controls::MultilineLable(lcd, labelWeatherDescriptionRect);
        controls.push_back(this->labelWeatherDescription);

        Controls::ControlRect labelDateRect = {
            0,
            digitalClockRect.leftUpY + digitalClockRect.height + 1,
            width,
            imageWeatherRect.leftUpY - (digitalClockRect.leftUpY + digitalClockRect.height) - 2};
        this->labelDate = new Controls::Label(lcd, labelDateRect);
        controls.push_back(this->labelDate);

        nowWeather = {99, F("weather not sync"), F("abort")};

        this->nowDate = {0, 0, 0};

        ReloadConfig();
    }

    void MainScreen::ReloadConfig()
    {
        String json = FileSystem::ReadFile(MAIN_SCREEN_CONFIG_PATH);
        if (json.isEmpty())
        {
            this->CreateDefaultConfig();
            json = FileSystem::ReadFile(MAIN_SCREEN_CONFIG_PATH);
        }

        const uint colorCount = 4;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
            CONFIG_ERROR_COLOR,
        };

        uint16_t clockMainColor;
        uint16_t clockSecondColor;
        uint16_t errorColor;

        uint16_t *colors[colorCount]{
            &this->backColor,
            &clockMainColor,
            &clockSecondColor,
            &errorColor};

        bool loadRes = DrawUtils::LoadColorsFromJson(json, colorNames, colors, colorCount);

        if (loadRes == false)
        {
            this->CreateDefaultConfig();
            this->ReloadConfig();
        }
        else
        {
            this->labelMessage->SetBackColor(this->backColor);
            this->labelMessage->SetMainColor(clockMainColor);

            this->labelDate->SetBackColor(this->backColor);
            this->labelDate->SetMainColor(clockMainColor);

            this->imageWeather->SetBackColor(this->backColor);
            this->imageWeather->SetMainColor(clockMainColor);

            this->labelTemp->SetBackColor(this->backColor);
            this->labelTemp->SetMainColor(clockMainColor);

            this->labelWeatherDescription->SetBackColor(this->backColor);
            this->labelWeatherDescription->SetMainColor(clockMainColor);

            this->digitalClock->SetBackColor(this->backColor);
            this->digitalClock->SetMainColor(clockMainColor);
            this->digitalClock->SetClockSecondColor(clockSecondColor);
        }

        for (const auto &control : controls)
        {
            control->ReDraw(true);
        }
    }

    void MainScreen::CreateDefaultConfig()
    {
        const uint configCount = 4;
        String configNames[configCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR,
            CONFIG_ERROR_COLOR};

        String datas[configCount]{
            DrawUtils::GetJsonColor(0, 0, 0),
            DrawUtils::GetJsonColor(0, 0, 255),
            DrawUtils::GetJsonColor(0, 0, 200),
            DrawUtils::GetJsonColor(255, 0, 0),
        };

        FileSystem::WriteFile(
            MAIN_SCREEN_CONFIG_PATH,
            JsonParser::BuildJson(configNames, datas, configCount));
    }

    void MainScreen::EnterFocus()
    {
        ClearScreen();
        for (const auto &control : controls)
        {
            control->ReDraw(true);
        }
    }

    void MainScreen::DrawWeather()
    {
        this->imageWeather->DrawImage(
            WeatherImages::GetImage(nowWeather.imageName),
            WeatherImages::ImageByteCount);

        char tempStr[] = {'+', '_', '_', 'c', '\0'};

        int tempAbs = nowWeather.temp >= 0 ? nowWeather.temp : -nowWeather.temp;
        if (tempAbs < 10)
        {
            tempStr[0] = ' ';
            tempStr[1] = nowWeather.temp > 0 ? '+' : '-';
        }
        else
        {
            tempStr[0] = nowWeather.temp > 0 ? '+' : '-';
            tempStr[1] = tempAbs / 10 + '0';
        }

        tempStr[2] = tempAbs % 10 + '0';

        this->labelTemp->DrawText(String(tempStr), Controls::Label::TextAlignment::Right);
        this->labelWeatherDescription->DrawText(this->nowWeather.description, Controls::Label::TextAlignment::Left);
    }

    void MainScreen::SetWeather(const Weather::WeatherData &data)
    {
        this->nowWeather = data;
        DrawWeather();
    }

    void MainScreen::SetMessage(const String &msg)
    {
        this->labelMessage->DrawText(msg, Controls::Label::TextAlignment::Center);
    }

    void MainScreen::SetTime(const Clock::Clock &clock)
    {
        auto time = clock.GetTime();
        this->digitalClock->SetTime(
            (byte)time.hour,
            (byte)time.minute,
            ((int)(time.second)) % 2 == 1);
        this->digitalClock->ReDraw();

        auto newDate = clock.GetDate();
        if (this->nowDate != newDate)
        {
            this->labelDate->DrawText(clock.GetDateString(), Controls::Label::TextAlignment::Center);
            this->nowDate = newDate;
        }
    }

    void MainScreen::SetTimeOk(bool isOk)
    {
    }

    MainScreen::~MainScreen()
    {
        delete labelMessage;
        delete labelDate;
        delete digitalClock;
        delete labelTemp;
        delete labelWeatherDescription;
    }
}