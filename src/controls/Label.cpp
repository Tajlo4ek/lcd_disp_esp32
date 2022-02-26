#include "Label.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{

    Label::Label(TFT_eSPI *lcd, ControlRect rect)
        : BaseControl(lcd, rect)
    {
        this->alignment = Controls::Label::TextAlignment::Left;
        this->text = "";
    }

    void Label::DrawText(const String &text, TextAlignment alignment)
    {
        this->text = text;
        this->alignment = alignment;
        ReDraw();
    }

    void Label::Draw(bool force)
    {
        ClearRect();
        SetViewPort();

        int textFont = 1;
        int fontSize = 1;

        for (const auto font : textFontSorted)
        {
            bool isFind = false;

            for (auto size : textSizeSorted)
            {
                lcd->setTextFont(font);
                lcd->setTextSize(size);

                int nowTextHeight = lcd->fontHeight();
                int nowTextWidth = lcd->textWidth(this->text);

                if (nowTextHeight != 0 && nowTextHeight <= controlRect.height && nowTextWidth <= controlRect.width)
                {
                    textFont = font;
                    fontSize = size;
                    isFind = true;
                    break;
                }
            }

            if (isFind)
            {
                break;
            }
        }

        lcd->setTextColor(mainColor, backColor);
        lcd->setTextFont(textFont);
        lcd->setTextSize(fontSize);

        int textWidth = lcd->textWidth(text);
        int drawX = 0;
        int drawY = (controlRect.height - lcd->fontHeight()) / 2;

        switch (this->alignment)
        {
        default:
        case TextAlignment::Left:
            drawX = 0;
            break;

        case TextAlignment::Center:
            drawX = (controlRect.width - textWidth) / 2;
            break;

        case TextAlignment::Right:
            drawX = controlRect.width - textWidth;
            break;
        }

        lcd->drawString(text, drawX, drawY);
    }

    Label::~Label()
    {
        this->text.clear();
    }
}