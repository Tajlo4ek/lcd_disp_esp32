#include "BaseScreen.h"

namespace Screens
{
    Screen::Screen(TFT_eSPI *lcd)
    {
        this->lcd = lcd;
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

    bool Screen::OnBtnLeftClick() { return false; }
    bool Screen::OnBtnRightClick() { return false; }
    bool Screen::OnBtnCenterClick() { return false; }
    bool Screen::OnBtnUpClick() { return false; }
    bool Screen::OnBtnDownClick() { return false; }
}