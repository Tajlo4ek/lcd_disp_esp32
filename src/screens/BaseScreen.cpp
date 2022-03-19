#include "BaseScreen.h"

namespace Screens
{
    Screen::Screen(TFT_eSPI *lcd, int width, int height)
    {
        this->lcd = lcd;
        lcd->resetViewport();

        this->width = width;
        this->height = height;
    }

    void Screen::LeaveFocus() {}

    String Screen::ParseMessage(const String &message)
    {
        return String();
    }

    void Screen::ClearScreen()
    {
        lcd->resetViewport();
        lcd->fillScreen(this->backColor);
    }

    void Screen::SetVisible(bool isVisible)
    {
        for (const auto &control : controls)
        {
            control->SetScreenVisible(isVisible);
        }

        if (isVisible == true)
        {
            EnterFocus();
        }
        else
        {
            LeaveFocus();
        }
    }
}