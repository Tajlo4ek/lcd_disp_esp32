#ifndef _BASE_SCREEN_H_
#define _BASE_SCREEN_H_

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "controls/BaseControl.h"
#include "functional"

namespace Screens
{
    typedef std::function<void()> OnScreenWorkEnd;

    class Screen
    {

    private:
        bool isVisible;

    protected:
        TFT_eSPI *lcd;
        int width;
        int height;
        uint16_t backColor;

        virtual void CreateDefaultConfig() = 0;
        void ClearScreen();

        virtual void EnterFocus() = 0;
        virtual void LeaveFocus();

        std::vector<Controls::BaseControl *> controls;

    public:
        virtual String ParseMessage(const String &message);
        virtual void ReloadConfig() = 0;

        void SetVisible(bool isVisible);

        Screen(TFT_eSPI *lcd, int width, int height);
    };
}

#endif