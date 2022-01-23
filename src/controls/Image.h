#ifndef _IMAGE_CONTROL_H_
#define _IMAGE_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    class Image : public BaseControl
    {
    private:
        const uint8_t *image;
        uint16_t imageLen;

    public:
        Image(TFT_eSPI *lcd, ControlRect rect);

        void ReDraw() override;
        void DrawImage(const uint8_t *image, const uint16_t imageLen);

        ~Image();
    };

}

#endif