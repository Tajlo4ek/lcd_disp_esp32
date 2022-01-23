#ifndef _BASE_CONTROL_H_
#define _BASE_CONTROL_H_

#include <TFT_eSPI.h>

namespace Controls
{
    struct ControlRect
    {
        int leftUpX;
        int leftUpY;
        int width;
        int height;
    };

    class BaseControl
    {
    private:
        bool isVisible;
        bool isScreenVisible;

    protected:
        ControlRect controlRect;
        TFT_eSPI *lcd;

        uint16_t backColor;
        uint16_t mainColor;

        void ClearRect();
        void SetViewPort();

        virtual void Draw() = 0;

    public:
        BaseControl(TFT_eSPI *lcd, ControlRect controlRect);

        virtual ~BaseControl();

        void ReDraw();

        void SetMainColor(uint16_t color);
        void SetBackColor(uint16_t color);

        void SetScreenVisible(bool val);
        void SetVisible(bool val);
    };

}

#endif