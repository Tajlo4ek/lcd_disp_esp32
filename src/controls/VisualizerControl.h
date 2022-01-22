#ifndef _VISUALIZER_CONTROL_H_
#define _VISUALIZER_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    class VisualizerControl : public BaseControl
    {
    private:
        uint16_t lowColor;
        uint16_t mediumColor;
        uint16_t highColor;
        uint16_t maxColor;

        int spectrumLineCount;
        byte *nowLeftSpectrum;
        byte *maxLeftSpectrumData;

        byte *nowRightSpectrum;
        byte *maxRightSpectrumData;

        int spectrumMaxSize;
        int spectrumMaxSizeDiv3;

    public:
        VisualizerControl(TFT_eSPI *lcd, ControlRect rect);
        ~VisualizerControl();

        void DrawSpectrum(byte *spectrumLeft, byte *spectrumRight);
        const int GetLineCount() const;
        const int GetMaxLineLength() const;

        void SetColors(uint16_t lowColor, uint16_t mediumColor, uint16_t highColor, uint16_t maxColor);

        void Reset();
        void ReDraw() override;
    };

}

#endif