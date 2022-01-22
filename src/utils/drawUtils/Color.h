#pragma once

#include "Arduino.h"

namespace DrawUtils
{
    struct ColorRGB
    {
        byte red;
        byte green;
        byte blue;
    };

    String GetJsonColor(const ColorRGB &color);
    String GetJsonColor(const byte r, const byte g, const byte b);

    const bool ColorFromJson(const String &json, ColorRGB &color);

    const uint16_t Get565Color(const byte r, const byte g, const byte b);
    const uint16_t Get565Color(const ColorRGB &color);

    const bool LoadColorsFromJson(const String &json, const String *colorNames, uint16_t **colors, const uint count);
}