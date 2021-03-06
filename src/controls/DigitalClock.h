#ifndef _DIGITAL_CLOCK_CONTROL_H_
#define _DIGITAL_CLOCK_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    class DigitalClock : public BaseControl
    {
    private:
        void DrawNum(byte num, int x, int y) const;

        enum Orientation
        {
            Horizontal,
            Vertical
        };

        void DrawBlock(int x, int y, uint16_t color, Orientation orientation) const;
        void DrawFastLine(int x, int y, int len, uint16_t color, Orientation orientation) const;

        uint16_t clockSecondColor;

        int numPositions[4];

        int blockWidth;
        int blockHeight;

        int dotCenterPosX;
        int dotCenterPosY;
        int dotDeltaY;
        int dotRadius;

        bool minutesChanged;
        byte nowMinutes;

        bool hoursChanged;
        byte nowHours;

        bool dotsChanged;
        bool needDots;

        void Draw(bool force) override;

    public:
        DigitalClock(TFT_eSPI *lcd, ControlRect rect);

        void SetClockSecondColor(uint16_t color);

        void SetTime(byte hours, byte minutes, bool needDots);
    };

}

#endif