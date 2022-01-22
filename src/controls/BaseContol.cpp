#include "BaseControl.h"

#include "utils/drawUtils/Color.h"

namespace Controls
{
    BaseControl::BaseControl(TFT_eSPI *lcd, ControlRect controlRect)
    {
        this->lcd = lcd;
        this->controlRect = controlRect;
        this->backColor = DrawUtils::Get565Color(0, 0, 0);
        this->mainColor = DrawUtils::Get565Color(0, 255, 0);

        this->isVisible = true;
        this->isScreenVisible = true;
    }

    BaseControl::~BaseControl()
    {
        this->lcd = nullptr;
    }

    void BaseControl::SetVisible(bool val)
    {
        if (isVisible == val)
        {
            return;
        }

        isVisible = val;
        ReDraw();
    }

    void BaseControl::SetScreenVisible(bool val)
    {
        this->isScreenVisible = val;
    }

    void BaseControl::SetMainColor(uint16_t color)
    {
        this->mainColor = color;
    }

    void BaseControl::SetBackColor(uint16_t color)
    {
        this->backColor = color;
    }

    void BaseControl::ClearRect()
    {
        if (isScreenVisible)
        {
            SetViewPort();
            lcd->fillRect(0, 0, controlRect.width, controlRect.height, backColor);
        }
    }

    void BaseControl::SetViewPort()
    {
        lcd->setViewport(controlRect.leftUpX, controlRect.leftUpY, controlRect.width, controlRect.height);
    }

}