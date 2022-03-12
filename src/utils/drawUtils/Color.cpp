#include "Color.h"

namespace DrawUtils
{
    bool GetColorPart(const String &data, byte &part);

    Json GetJsonColor(const ColorRGB &color)
    {
        return GetJsonColor(color.red, color.green, color.blue);
    }

    Json GetJsonColor(const byte r, const byte g, const byte b)
    {
        std::vector<String> names{F("red"), F("green"), F("blue")};
        std::vector<String> data{String(r), String(g), String(b)};
        return Json(names, data);
    }

    const bool ColorFromJson(const Json &json, ColorRGB &color)
    {
        return GetColorPart(json[F("red")].ToString(), color.red) &&
               GetColorPart(json[F("green")].ToString(), color.green) &&
               GetColorPart(json[F("blue")].ToString(), color.blue);
    }

    const uint16_t Get565Color(const ColorRGB &color)
    {
        return Get565Color(color.red, color.green, color.blue);
    }

    const uint16_t Get565Color(const byte r, const byte g, const byte b)
    {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    bool GetColorPart(const String &data, byte &part)
    {
        if (data.isEmpty())
        {
            return false;
        }

        int clr = data.toInt();
        if (clr >= 0 && clr <= 255)
        {
            part = clr;
            return true;
        }
        return false;
    }

    const bool ColorsFromJson(const Json &json, const String *colorNames, uint16_t **colors, const uint count)
    {
        for (uint colorNum = 0; colorNum < count; colorNum++)
        {
            if (json.ContainsName(colorNames[colorNum]) == false)
            {
                return false;
            }

            auto colorData = json[colorNames[colorNum]];

            ColorRGB color;
            if (DrawUtils::ColorFromJson(colorData, color) == false)
            {
                return false;
            }
            *colors[colorNum] = Get565Color(color);
        }

        return true;
    }
}