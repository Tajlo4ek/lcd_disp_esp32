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
            Auto,
            Small = 1,
            Normal = 2,
            Big = 4,
        };

        Label(TFT_eSPI *lcd, ControlRect rect, TextSize size = TextSize::Auto);
        ~Label();

        void DrawText(const String &text, TextAlignment alignment);

    protected:
        String text;
        TextSize size;
        TextAlignment alignment;

        static const std::vector<TextSize> textSizeSorted;

        virtual void Draw(bool force);
    };

}

#endif