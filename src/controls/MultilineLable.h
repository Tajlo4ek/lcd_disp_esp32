#ifndef _MULTILINE_LABEL_CONTROL_H_
#define _MULTILINE_LABEL_CONTROL_H_

#include "Label.h"

namespace Controls
{
    class MultilineLable : public Label
    {
    private:
        void Draw(bool force) override;

    public:
        MultilineLable(TFT_eSPI *lcd, ControlRect rect);
    };

}

#endif