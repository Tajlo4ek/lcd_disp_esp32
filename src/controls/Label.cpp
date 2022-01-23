#include "Label.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{
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

        lcd->setTextSize((int)size / 8);
        lcd->setTextColor(mainColor, backColor);

        if ((int)this->size > controlRect.height)
        {
            lcd->setTextSize(1);
            lcd->drawString(F("size to big"), 0, 0);
            return;
        }

        String buf = text;
        int textWidth = lcd->textWidth(text);
        while (textWidth > controlRect.width)
        {
            buf.remove(buf.length() - 1);
            textWidth = lcd->textWidth(buf);
        }

        switch (this->alignment)
        {
        default:
        case TextAlignment::Left:
            lcd->drawString(buf, 0, 0);
            break;

        case TextAlignment::Center:
            lcd->drawString(text, (controlRect.width - textWidth) / 2, 0);
            break;

        case TextAlignment::Right:
            lcd->drawString(buf, controlRect.width - textWidth, 0);
            break;
        }
    }

    Label::~Label()
    {
        this->text.clear();
    }
}