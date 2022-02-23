#include "MultilineLable.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{
    MultilineLable::MultilineLable(TFT_eSPI *lcd, ControlRect rect)
        : Label(lcd, rect)
    {
    }

    void MultilineLable::Draw(bool force)
    {
        ClearRect();
        SetViewPort();

        lcd->setTextColor(this->mainColor, this->backColor);

        for (const auto font : fontSizeSorted)
        {
            lcd->setTextFont(font);
            int fontHeight = lcd->fontHeight();

            String text = this->text + ' ';
            String buf;

            std::vector<String> lines;

            int nowIndex = 0;
            while (true)
            {
                int ind = text.indexOf(' ', nowIndex);
                if (ind == -1)
                {
                    if (buf.isEmpty() == false)
                    {
                        lines.push_back(buf);
                    }
                    break;
                }

                String sub = text.substring(nowIndex, ind);

                if (lcd->textWidth(buf) + lcd->textWidth(sub) > controlRect.width)
                {
                    lines.push_back(buf);
                    buf = sub;
                }
                else
                {
                    buf += sub;
                    buf += ' ';
                }

                nowIndex = ind + 1;
            }

            int totalHeight = lines.size() * fontHeight;
            if (totalHeight <= controlRect.height)
            {
                int offsetY = (controlRect.height - totalHeight) / 2;

                for (int i = 0; i < lines.size(); i++)
                {
                    lcd->drawString(lines[i], 0, offsetY + i * fontHeight);
                }
                break;
            }
        }
    }

}