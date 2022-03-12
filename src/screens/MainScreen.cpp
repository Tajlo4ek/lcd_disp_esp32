#include "MainScreen.h"

#include "utils/FileNames.h"
#include "utils/Commands.h"
#include "utils/weather/WeatherImages.h"

#include "utils/fileSystem/FileSystem.h"
#include "utils/json/Json.h"
#include "utils/DrawUtils/Color.h"

namespace Screens
{
#define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_CLOCK_MAIN_COLOR F("clockMainColor")
#define CONFIG_CLOCK_SECOND_COLOR F("clockSecondColor")

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

        int imageSize = height * 3 / 10;
        Controls::ControlRect imageWeatherRect = {5, height - imageSize, imageSize, imageSize};
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
        this->isTimeSync = false;

        ReloadConfig();
    }

    void MainScreen::ReloadConfig()
    {
        String jsonString = FileSystem::ReadFile(MAIN_SCREEN_CONFIG_PATH);
        if (jsonString.isEmpty())
        {
            this->CreateDefaultConfig();
            jsonString = FileSystem::ReadFile(MAIN_SCREEN_CONFIG_PATH);
        }

        Json json(jsonString);

        const uint colorCount = 3;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR};

        uint16_t clockMainColor;
        uint16_t clockSecondColor;

        uint16_t *colors[colorCount]{
            &this->backColor,
            &clockMainColor,
            &clockSecondColor};

        bool loadRes = DrawUtils::ColorsFromJson(json, colorNames, colors, colorCount);

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

            for (const auto &control : controls)
            {
                control->ReDraw(true);
            }
        }
    }

    void MainScreen::CreateDefaultConfig()
    {
        std::vector<String> configNames{
            CONFIG_BACK_COLOR,
            CONFIG_CLOCK_MAIN_COLOR,
            CONFIG_CLOCK_SECOND_COLOR};

        std::vector<String> datas{
            DrawUtils::GetJsonColor(0, 0, 0).ToString(),
            DrawUtils::GetJsonColor(0, 0, 255).ToString(),
            DrawUtils::GetJsonColor(0, 0, 200).ToString()};

        FileSystem::WriteFile(
            MAIN_SCREEN_CONFIG_PATH,
            Json(configNames, datas).ToString());
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
            WeatherImages::imageWidth,
            WeatherImages::imageHeight,
            WeatherImages::imageByteCount);

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

    void MainScreen::SetTime(const Clock::Time &time, const Clock::Date &date)
    {
        this->digitalClock->SetTime(
            (byte)time.hour,
            (byte)time.minute,
            ((int)(time.second)) % 2 == 1);
        this->digitalClock->ReDraw();

        if (this->nowDate != date)
        {
            this->nowDate = date;
            DrawDate();
        }
    }

    void MainScreen::DrawDate()
    {
        String dateString;

        if (this->isTimeSync)
        {
            char dateBuf[11];

            dateBuf[0] = this->nowDate.day / 10 + '0';
            dateBuf[1] = this->nowDate.day % 10 + '0';
            dateBuf[2] = '.';
            dateBuf[3] = this->nowDate.month / 10 + '0';
            dateBuf[4] = this->nowDate.month % 10 + '0';
            dateBuf[5] = '.';

            int year = this->nowDate.year;
            dateBuf[6] = year / 1000 + '0';
            year %= 1000;

            dateBuf[7] = year / 100 + '0';
            year %= 100;

            dateBuf[8] = year / 10 + '0';
            dateBuf[9] = year % 10 + '0';
            dateBuf[10] = '\0';

            dateString = dateBuf;
        }
        else
        {
            dateString = F("Time not sync");
        }

        this->labelDate->DrawText(String(dateString), Controls::Label::TextAlignment::Center);
    }

    void MainScreen::SetTimeOk(bool isOk)
    {
        this->isTimeSync = isOk;
        DrawDate();
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