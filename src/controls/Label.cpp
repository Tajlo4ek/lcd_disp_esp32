#include "Label.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{
    const std::vector<Label::TextSize> Label::textSizeSorted = {Label::TextSize::Big, Label::TextSize::Normal, Label::TextSize::Small};

    Label::Label(TFT_eSPI *lcd, ControlRect rect, TextSize size)
        : BaseControl(lcd, rect)
    {
        this->alignment = Controls::Label::TextAlignment::Left;
        this->text = "";
        this->size = size;
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

        TextSize nowSize = this->size;
        if (nowSize == TextSize::Auto)
        {
            for (const auto textSize : textSizeSorted)
            {
                if (lcd->fontHeight(textSize) <= controlRect.height)
                {
                    nowSize = textSize;
                    break;
                }
            }
        }

        lcd->setTextColor(mainColor, backColor);
        lcd->setTextFont(nowSize);

        if (lcd->fontHeight() > controlRect.height)
        {
            lcd->setTextSize(1);
            lcd->drawString(F("font to big"), 0, 0);
            return;
        }

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