#pragma once

#include "Arduino.h"
#include "utils/json/Json.h"

namespace DrawUtils
{
    struct ColorRGB
    {
        byte red;
        byte green;
        byte blue;
    };

    Json GetJsonColor(const ColorRGB &color);
    Json GetJsonColor(const byte r, const byte g, const byte b);

    const uint16_t Get565Color(const byte r, const byte g, const byte b);
    const uint16_t Get565Color(const ColorRGB &color);

    const bool ColorFromJson(const Json &json, ColorRGB &color);
    const bool ColorsFromJson(const Json &json, const String *colorNames, uint16_t **colors, const uint count);
}