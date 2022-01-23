#ifndef _LABEL_CONTROL_H_
#define _LABEL_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    class Label : public BaseControl
    {

    public:
        enum TextAlignment
        {
            Left,
            Center,
            Right
        };

        enum TextSize
        {
            Small = 8,
            Big = 16,
        };

        Label(TFT_eSPI *lcd, ControlRect rect, TextSize size);
        ~Label();

        void DrawText(const String &text, TextAlignment alignment);

    protected:
        String text;
        TextSize size;
        TextAlignment alignment;

        virtual void Draw(bool force);
    };

}

#endif