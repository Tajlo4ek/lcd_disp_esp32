#ifndef _LABEL_CONTROL_H_
#define _LABEL_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    const int textFontSorted[] = {4, 2, 1};
    const int textSizeSorted[] = {4, 2, 1};

    class Label : public BaseControl
    {

    public:
        enum TextAlignment
        {
            Left,
            Center,
            Right
        };

        Label(TFT_eSPI *lcd, ControlRect rect);
        ~Label();

        void DrawText(const String &text, TextAlignment alignment);

    protected:
        String text;
        TextAlignment alignment;

        virtual void Draw(bool force);
    };
}

#endif