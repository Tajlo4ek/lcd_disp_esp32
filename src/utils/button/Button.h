#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <Arduino.h>

#include <functional>

namespace Utils
{
    enum ButtonName : byte
    {
        NameOk,
        NameRight,
        NameLeft,
        NameUp,
        NameDown
    };

    class Button
    {
    private:
        unsigned long lastClick;
        ButtonName name;

        std::function<void()> callback;

    public:
        Button();
        void CheckClick();
        void SetClickCallback(std::function<void()> callback);
    };

}

#endif