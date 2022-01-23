#ifndef _CHART_CONTROL_H_
#define _CHART_CONTROL_H_

#include "BaseControl.h"

namespace Controls
{
    class Chart : public BaseControl
    {
    private:
        uint16_t mainColor;
        uint16_t secondColor;

        int valueCount;
        int *values;
        float scaleVal;

        void Draw();

    public:
        Chart(TFT_eSPI *lcd, ControlRect rect);
        ~Chart();

        void AddValue(const float value);
        void ResetValues();
    };

}

#endif