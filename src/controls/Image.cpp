#include "Image.h"

namespace Controls
{
    Image::Image(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->image = nullptr;
        this->imageW = 0;
        this->imageH = 0;
    }

    void Image::Draw(bool force)
    {
        if (this->image == nullptr || this->imageH == 0 || this->imageW == 0)
        {
            ClearRect();
            return;
        }

        SetViewPort();

        std::vector<uint32_t> pixels;
        auto colorMain = lcd->color16to24(this->mainColor);
        auto colorBack = lcd->color16to24(this->backColor);
        int totalPixel = this->imageH * this->imageW;

        for (int i = 0; i < this->bytesCount; i++)
        {
            int num = this->image[i];

            for (int y = 0; y < sizeof(this->image[0]) * 8; y++)
            {
                pixels.push_back(num % 2 == 0 ? colorBack : colorMain);
                num /= 2;
                totalPixel--;

                if (totalPixel == 0)
                {
                    break;
                }
            }

            if (totalPixel == 0)
            {
                break;
            }
        }

        float xRatio = ((float)(this->imageW - 1)) / this->controlRect.width;
        float yRatio = ((float)(this->imageH - 1)) / this->controlRect.height;

        for (int i = 0; i < this->controlRect.height; i++)
        {
            for (int j = 0; j < this->controlRect.width; j++)
            {
                int x = (int)(xRatio * j);
                int y = (int)(yRatio * i);
                float xDiff = (xRatio * j) - x;
                float yDiff = (yRatio * i) - y;
                int index = (y * this->imageW + x);
                auto a = pixels[index];
                auto b = pixels[index + 1];
                auto c = pixels[index + this->imageW];
                auto d = pixels[index + this->imageW + 1];

                int blue = (a & 0xff) * (1 - xDiff) * (1 - yDiff) + (b & 0xff) * (xDiff) * (1 - yDiff) +
                           (c & 0xff) * (yDiff) * (1 - xDiff) + (d & 0xff) * (xDiff * yDiff);

                int green = ((a >> 8) & 0xff) * (1 - xDiff) * (1 - yDiff) + ((b >> 8) & 0xff) * (xDiff) * (1 - yDiff) +
                            ((c >> 8) & 0xff) * (yDiff) * (1 - xDiff) + ((d >> 8) & 0xff) * (xDiff * yDiff);

                int red = ((a >> 16) & 0xff) * (1 - xDiff) * (1 - yDiff) + ((b >> 16) & 0xff) * (xDiff) * (1 - yDiff) +
                          ((c >> 16) & 0xff) * (yDiff) * (1 - xDiff) + ((d >> 16) & 0xff) * (xDiff * yDiff);

                lcd->drawPixel(j, i, lcd->color565(red, green, blue));
            }
        }
    }

    void Image::DrawImage(const uint8_t *image, const int imageW, const int imageH, const int bytesCount)
    {
        this->image = image;
        this->imageW = imageW;
        this->imageH = imageH;
        this->bytesCount = bytesCount;

        ReDraw();
    }

    Image::~Image()
    {
    }
}