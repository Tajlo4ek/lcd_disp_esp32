#ifndef _IMAGE_CONTROL_H_
#define _IMAGE_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    class Image : public BaseControl
    {
    private:
        const uint8_t *image;
        int imageW;
        int imageH;
        int bytesCount;

        void Draw(bool force) override;

    public:
        Image(TFT_eSPI *lcd, ControlRect rect);

        void DrawImage(const uint8_t *image, const int imageW, const int imageH, const int bytesCount);

        ~Image();
    };

}

#endif