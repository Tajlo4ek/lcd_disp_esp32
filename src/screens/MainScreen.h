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

    private:
        Weather::CurrentWeaterData nowWeather;
        Clock::Date nowDate;

        Controls::Label *labelMessage;

        Controls::Label *labelDate;
        Controls::DigitalClock *digitalClock;

        Controls::Image *imageWeather;
        Controls::Label *labelTemp;
        Controls::MultilineLable *labelWeatherDescription;

        bool isTimeSync;

        void DrawWeather();
        void DrawDate();

        void CreateDefaultConfig() override;
        void EnterFocus() override;

    public:
        void SetWeather(const Weather::CurrentWeaterData &data);
        void SetTime(const Clock::Time &time, const Clock::Date &date);

        void SetMessage(const String &msg);
        void SetTimeOk(bool isOk);

        void ReloadConfig() override;

        MainScreen(TFT_eSPI *lcd, int width, int height);
        ~MainScreen();
    };
}

#endif