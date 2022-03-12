#include "VisualizerScreen.h"

#include "utils/Commands.h"
#include "utils/FileNames.h"

#include "utils/fileSystem/FileSystem.h"
#include "utils/json/Json.h"
#include "utils/drawUtils/Color.h"

namespace Screens
{

#define CONFIG_BACK_COLOR F("backColor")
#define CONFIG_LOW_COLOR F("lowColor")
#define CONFIG_MEDIUM_COLOR F("mediumColor")
#define CONFIG_HIGH_COLOR F("highColor")
#define CONFIG_MAX_COLOR F("maxColor")

    VisualizerScreen::VisualizerScreen(TFT_eSPI *lcd, int width, int height)
        : Screen(lcd, width, height)
    {
        Controls::ControlRect rect = {5, 0, width - 10, height};
        this->visualizer = new Controls::VisualizerControl(lcd, rect);

        this->spectrumLen = this->visualizer->GetLineCount();
        this->leftSpectrumParseData = new byte[this->spectrumLen];
        this->rightSpectrumParseData = new byte[this->spectrumLen];

        controls.push_back(this->visualizer);

        this->ReloadConfig();
    }

    void VisualizerScreen::ReloadConfig()
    {
        auto jsonString = FileSystem::ReadFile(SPECTRUM_CONFIG_PATH);
        if (jsonString.isEmpty())
        {
            this->CreateDefaultConfig();
            jsonString = FileSystem::ReadFile(SPECTRUM_CONFIG_PATH);
        }

        Json json(jsonString);

        const uint colorCount = 5;
        String colorNames[colorCount]{
            CONFIG_BACK_COLOR,
            CONFIG_LOW_COLOR,
            CONFIG_MEDIUM_COLOR,
            CONFIG_HIGH_COLOR,
            CONFIG_MAX_COLOR,
        };

        uint16_t lowColor;
        uint16_t mediumColor;
        uint16_t highColor;
        uint16_t maxColor;

        uint16_t *colors[colorCount]{
            &this->backColor,
            &lowColor,
            &mediumColor,
            &highColor,
            &maxColor,
        };

        bool loadRes = DrawUtils::ColorsFromJson(json, colorNames, colors, colorCount);

        if (loadRes == false)
        {
            this->CreateDefaultConfig();
            this->ReloadConfig();
        }
        else
        {
            this->visualizer->SetColors(lowColor, mediumColor, highColor, maxColor);
            this->visualizer->SetBackColor(this->backColor);
        }

        for (const auto &control : controls)
        {
            control->ReDraw(true);
        }
    }

    void VisualizerScreen::CreateDefaultConfig()
    {
        std::vector<String> configNames{
            CONFIG_BACK_COLOR,
            CONFIG_LOW_COLOR,
            CONFIG_MEDIUM_COLOR,
            CONFIG_HIGH_COLOR,
            CONFIG_MAX_COLOR,
        };

        std::vector<String> datas{
            DrawUtils::GetJsonColor(0, 0, 0).ToString(),
            DrawUtils::GetJsonColor(0, 255, 0).ToString(),
            DrawUtils::GetJsonColor(255, 255, 0).ToString(),
            DrawUtils::GetJsonColor(255, 0, 0).ToString(),
            DrawUtils::GetJsonColor(0, 255, 255).ToString(),
        };

        FileSystem::WriteFile(
            SPECTRUM_CONFIG_PATH,
            Json(configNames, datas).ToString());
    }

    String VisualizerScreen::ParseMessage(const String &message)
    {
        if (message.startsWith(COMMAND_SET_MODE_SPECTRUM))
        {
            return GetSpectrumData();
        }
        else if (message.startsWith(COMMAND_SEND_SPECTRUM_DATA))
        {
            ParseSpectrum(message);
        }
        return String();
    }

    String VisualizerScreen::GetSpectrumData()
    {
        String data = COMMAND_SEND_LINE_COUNT;
        data += visualizer->GetLineCount();
        data += COMMAND_STOP_CHAR;
        data += COMMAND_SET_MAX_SPECTRUM_DATA;
        data += visualizer->GetMaxLineLength();
        return data;
    }

    void VisualizerScreen::ParseSpectrum(const String &data)
    {
        int dateLen = data.length();

        bool parseOk = true;
        byte next = 0;
        int spNum = 0;
        for (int pos = String(COMMAND_SEND_SPECTRUM_DATA).length(); pos < dateLen; pos++)
        {
            char ch = data[pos];
            if (ch != COMMAND_SPLIT_CHAR)
            {
                next *= 10;
                next += ch - '0';
            }
            else
            {
                if (spNum >= 2 * this->spectrumLen)
                {
                    parseOk = false;
                    break;
                }

                if (spNum < spectrumLen)
                {
                    leftSpectrumParseData[spNum] = next;
                }
                else
                {
                    rightSpectrumParseData[spNum - spectrumLen] = next;
                }

                next = 0;
                spNum++;
            }
        }

        if (spNum != 2 * this->spectrumLen)
        {
            parseOk = false;
        }

        if (parseOk)
        {
            visualizer->SetSpectrum(leftSpectrumParseData, rightSpectrumParseData);
            visualizer->ReDraw();
        }
    }

    void VisualizerScreen::EnterFocus()
    {
        ClearScreen();

        this->visualizer->Reset();

        for (const auto &control : controls)
        {
            control->ReDraw(true);
        }
    }

    VisualizerScreen::~VisualizerScreen()
    {
        delete this->visualizer;

        delete[] leftSpectrumParseData;
        delete[] rightSpectrumParseData;
    }
}