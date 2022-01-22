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
        uint16_t backColor;

        virtual void CreateDefaultConfig() = 0;
        void ClearScreen();

        virtual void EnterFocus() = 0;
        virtual void LeaveFocus();

        std::vector<Controls::BaseControl *> controls;

    public:
        virtual String ParseMessage(const String &message);
        virtual void ReloadConfig() = 0;
        virtual void ReDraw() = 0;

        void SetVisible(bool isVisible);

        Screen(TFT_eSPI *lcd);

        virtual bool OnBtnLeftClick();
        virtual bool OnBtnRightClick();
        virtual bool OnBtnCenterClick();
    };
}

#endif