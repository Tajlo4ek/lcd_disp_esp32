#ifndef _VISUALIZER_SCREEN_H_
#define _VISUALIZER_SCREEN_H_

#include "BaseScreen.h"

#include "controls/visualizerControl.h"

namespace Screens
{
    class VisualizerScreen : public Screen
    {
    private:
        String GetSpectrumData();
        void ParseSpectrum(const String &data);

        Controls::VisualizerControl *visualizer;

        byte *leftSpectrumParseData;
        byte *rightSpectrumParseData;
        int spectrumLen;

        void CreateDefaultConfig() override;

        void EnterFocus() override;

    public:
        String ParseMessage(const String &message) override;
        void ReloadConfig() override;

        VisualizerScreen(TFT_eSPI *lcd, int width, int height);
        ~VisualizerScreen();
    };
}

#endif