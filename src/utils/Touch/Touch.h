#ifndef _UTILS_TOUCH_H_
#define _UTILS_TOUCH_H_

#include <TFT_eSPI.h>

namespace Touch
{
    struct CalibrateConfig
    {
        int minX;
        int maxX;
        int minY;
        int maxY;
        bool isRotated;
        bool isXInverse;
        bool isYInverse;
    };

    struct Point
    {
        int x;
        int y;
    };

    class TouchScreen
    {
    private:
        CalibrateConfig config;
        TFT_eSPI *lcd;

        int screenWidth;
        int screenHeight;

    public:
        TouchScreen(TFT_eSPI *lcd);
        void Calibrate();
        const bool GetTouch(Point &point) const;

        void SetConfig(const CalibrateConfig &config);
        const CalibrateConfig GetConfig() const;
    };

}

#endif
