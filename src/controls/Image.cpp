#include "Image.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{

    Image::Image(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->imageLen = 0;
    }

    void Image::ReDraw()
    {
        ClearRect();

        if (this->isVisible == false || isScreenVisible == false || this->imageLen == 0)
        {
            return;
        }

        int x = 0;
        int y = 0;

        for (uint16_t itemNum = 0; itemNum < this->imageLen; itemNum++)
        {
            uint8_t item = this->image[itemNum];
            for (byte i = 0; i < 8; i++)
            {
                if (x >= controlRect.width)
                {
                    x = 0;
                    y++;
                    if (y >= controlRect.height)
                    {
                        return;
                    }
                }

                this->lcd->drawPixel(
                    x,
                    y,
                    item % 2 == 1 ? this->mainColor : this->backColor);

                item /= 2;
                x++;
            }
        }
    }

    void Image::DrawImage(const uint8_t *image, const uint16_t imageLen)
    {
        this->image = image;
        this->imageLen = imageLen;

        ReDraw();
    }

    Image::~Image()
    {
        delete[] this->image;
    }
}