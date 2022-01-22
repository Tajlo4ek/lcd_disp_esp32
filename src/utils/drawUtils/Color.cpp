#include "Color.h"

#include "utils/json/JsonParser.h"

namespace DrawUtils
{
    bool GetColorPart(const String &data, byte &part);

    String GetJsonColor(const ColorRGB &color)
    {
        return GetJsonColor(color.red, color.green, color.blue);
    }

    String GetJsonColor(const byte r, const byte g, const byte b)
    {
        const int dataCount = 3;
        String names[dataCount]{F("red"), F("green"), F("blue")};
        String data[dataCount]{String(r), String(g), String(b)};
        return JsonParser::BuildJson(names, data, dataCount);
    }

    const bool ColorFromJson(const String &json, ColorRGB &color)
    {
        if (json.isEmpty())
        {
            return false;
        }

        return GetColorPart(JsonParser::GetJsonData(json, F("red")), color.red) &&
               GetColorPart(JsonParser::GetJsonData(json, F("green")), color.green) &&
               GetColorPart(JsonParser::GetJsonData(json, F("blue")), color.blue);
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

    const bool LoadColorsFromJson(const String &json, const String *colorNames, uint16_t **colors, const uint count)
    {
        for (uint colorNum = 0; colorNum < count; colorNum++)
        {
            bool isOk = false;
            auto colorData = JsonParser::GetJsonData(json, colorNames[colorNum], isOk);
            if (isOk == false)
            {
                return false;
            }

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