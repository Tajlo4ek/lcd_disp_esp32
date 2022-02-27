#include "DigitalClock.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{

    DigitalClock::DigitalClock(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->clockSecondColor = DrawUtils::Get565Color(0, 200, 0);
        this->nowMinutes = 0;
        this->nowHours = 0;
        this->needDots = false;

        this->minutesChanged = false;
        this->hoursChanged = false;
        this->dotsChanged = false;

        int numSpace = rect.width * 2 / 100;

        int numWidth = (rect.width - numSpace * 6) / 5;
        while (true)
        {
            this->blockWidth = numWidth / 3;
            while (this->blockWidth % 2 == 0)
            {
                this->blockWidth--;
            }
            this->blockHeight = numWidth - this->blockWidth / 2 * 2;

            int numHeight = blockHeight * 2 + this->blockWidth + 2;

            if (numHeight <= rect.height)
            {
                break;
            }

            numWidth--;
        }

        numWidth += 2;

        this->dotCenterPosX = rect.width / 2;
        this->dotCenterPosY = rect.height / 2;

        this->dotDeltaY = rect.height / 5;
        this->dotRadius = this->dotDeltaY / 2;

        if (this->dotRadius < 2)
        {
            this->dotRadius = 2;
        }

        this->numPositions[0] = this->dotCenterPosX - this->dotRadius - (numSpace + numWidth) * 2;
        this->numPositions[1] = this->dotCenterPosX - this->dotRadius - (numSpace + numWidth) * 1;

        this->numPositions[2] = this->dotCenterPosX + this->dotRadius + numSpace;
        this->numPositions[3] = this->dotCenterPosX + this->dotRadius + numSpace * 2 + numWidth;
    }

    void DigitalClock::SetClockSecondColor(uint16_t color)
    {
        this->clockSecondColor = color;
    }

    void DigitalClock::Draw(bool force)
    {
        this->SetViewPort();

        if (this->hoursChanged || force)
        {
            this->hoursChanged = false;

            DrawNum(this->nowHours / 10, numPositions[0], 0);
            DrawNum(this->nowHours % 10, numPositions[1], 0);
        }

        if (this->minutesChanged || force)
        {
            this->minutesChanged = false;

            DrawNum(this->nowMinutes / 10, numPositions[2], 0);
            DrawNum(this->nowMinutes % 10, numPositions[3], 0);
        }

        if (this->dotsChanged || force)
        {
            this->dotsChanged = false;

            uint16_t dotColor = this->needDots ? this->mainColor : this->backColor;

            this->lcd->fillEllipse(
                this->dotCenterPosX,
                this->dotCenterPosY - this->dotDeltaY,
                this->dotRadius,
                this->dotRadius,
                dotColor);

            this->lcd->fillEllipse(
                this->dotCenterPosX,
                this->dotCenterPosY + this->dotDeltaY,
                this->dotRadius,
                this->dotRadius,
                dotColor);
        }
    }

    void DigitalClock::SetTime(byte hours, byte minutes, bool needDots)
    {
        if (this->nowHours != hours)
        {
            this->nowHours = hours;
            this->hoursChanged = true;
        }

        if (this->nowMinutes != minutes)
        {
            this->nowMinutes = minutes;
            this->minutesChanged = true;
        }

        if (this->needDots != needDots)
        {
            this->needDots = needDots;
            this->dotsChanged = true;
        }
    }

    void DigitalClock::DrawNum(byte num, int x, int y) const
    {
        if (num > 9)
        {
            return;
        }

        // top
        uint16_t color = (num != 1 && num != 4) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockWidth / 2 + 1,
            y - 1,
            color,
            Orientation::Horizontal);

        // top left
        color = (num != 1 && num != 2 && num != 3 && num != 7) ? this->mainColor : this->backColor;
        DrawBlock(
            x - 1,
            y + this->blockWidth / 2 + 1,
            color,
            Orientation::Vertical);

        // top right
        color = (num != 5 && num != 6) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockHeight,
            y + this->blockWidth / 2 + 1,
            color,
            Orientation::Vertical);

        // center
        color = (num > 1 && num != 7) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockWidth / 2 + 1,
            y + this->blockHeight,
            color,
            Orientation::Horizontal);

        // bottom left
        color = (num == 0 || num == 2 || num == 6 || num == 8) ? this->mainColor : this->backColor;
        DrawBlock(
            x - 1,
            y + this->blockWidth / 2 + this->blockHeight + 2,
            color,
            Orientation::Vertical);

        // bottom right
        color = (num != 2) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockHeight,
            y + this->blockWidth / 2 + this->blockHeight + 2,
            color,
            Orientation::Vertical);

        // bottom
        color = (num != 1 && num != 4 && num != 7) ? this->mainColor : this->backColor;
        DrawBlock(
            x + this->blockWidth / 2 + 1,
            y + (this->blockHeight + 1) * 2 - 1,
            color,
            Orientation::Horizontal);
    }

    void DigitalClock::DrawBlock(int x, int y, uint16_t color, Orientation orientation) const
    {
        const int blockWidthDiv2 = this->blockWidth / 2;
        int offsetX = 0;
        int offsetY = 0;
        int signX = 0;
        int signY = 0;

        if (orientation == Orientation::Horizontal)
        {
            offsetX = x;
            offsetY = y + blockWidthDiv2 + 1;

            signX = 1;
            signY = -1;
        }
        else
        {
            offsetX = x + blockWidthDiv2 + 1;
            offsetY = y;

            signX = -1;
            signY = 1;
        }

        DrawFastLine(
            offsetX,
            offsetY,
            this->blockHeight,
            color,
            orientation);

        DrawFastLine(
            offsetX + blockWidthDiv2 * signX,
            offsetY + blockWidthDiv2 * signY,
            this->blockHeight - blockWidthDiv2 * 2,
            this->clockSecondColor,
            orientation);

        DrawFastLine(
            offsetX + blockWidthDiv2,
            offsetY + blockWidthDiv2,
            this->blockHeight - blockWidthDiv2 * 2,
            this->clockSecondColor,
            orientation);

        for (int i = 1; i < blockWidthDiv2; i++)
        {
            DrawFastLine(
                offsetX + i * signX,
                offsetY + i * signY,
                this->blockHeight - i * 2,
                color,
                orientation);

            DrawFastLine(
                offsetX + i,
                offsetY + i,
                this->blockHeight - i * 2,
                color,
                orientation);
        }
    }

    void DigitalClock::DrawFastLine(int x, int y, int len, uint16_t color, Orientation orientation) const
    {
        if (orientation == Orientation::Horizontal)
        {
            lcd->drawFastHLine(x + 1, y, len - 2, color);
            lcd->drawPixel(x + len - 1, y, this->clockSecondColor);
        }
        else
        {
            lcd->drawFastVLine(x, y + 1, len - 2, color);
            lcd->drawPixel(x, y + len - 1, this->clockSecondColor);
        }
        lcd->drawPixel(x, y, this->clockSecondColor);
    }
}