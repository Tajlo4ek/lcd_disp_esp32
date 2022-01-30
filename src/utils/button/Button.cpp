#include "Button.h"

#define CLICK_DELAY 200

namespace Utils
{
    Button::Button()
    {
        this->name = name;
        this->callback = nullptr;
    }

    void Button::CheckClick()
    {
        unsigned long time = millis();
        bool isClick = (time - lastClick) > CLICK_DELAY;
        lastClick = time;

        if (isClick && callback != nullptr)
        {
            callback();
        }
    }

    void Button::SetClickCallback(std::function<void()> callback)
    {
        this->callback = callback;
    }

}