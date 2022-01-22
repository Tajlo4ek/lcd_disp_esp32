#ifndef _MAIN_SCREEN_H_
#define _MAIN_SCREEN_H_

#include "BaseScreen.h"

#include "controls/Label.h"
#include "controls/DigitalClock.h"
#include "controls/Image.h"
#include "controls/MultilineLable.h"
#include "utils/weather/Weather.h"
#include "utils/clock/Clock.h"

namespace Screens
{
    class MainScreen : public Screen
    {
    public:
        void SetWeather(const Weather::WeatherData &data);
        void SetTime(const Clock::Clock &clock);

        void SetMessage(const String &msg);
        void SetTimeOk(bool isOk);

    private:
        Weather::WeatherData nowWeather;
        Clock::Date nowDate;

        Controls::Label *labelMessage;

        Controls::Label *labelDate;
        Controls::Label *labelTimeSync;
        Controls::DigitalClock *digitalClock;

        Controls::Image *imageWeather;
        Controls::Label *labelTemp;
        Controls::MultilineLable *labelWeatherDescription;

        void DrawWeather();

        void CreateDefaultConfig() override;
        void EnterFocus() override;

    public:
        void LeaveFocus() override;
        void ReloadConfig() override;
        void ReDraw() override;

        MainScreen(TFT_eSPI *lcd);
        ~MainScreen();
    };
}

#endif