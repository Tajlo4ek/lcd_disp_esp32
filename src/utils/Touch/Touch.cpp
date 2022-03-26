#include "Touch.h"

#define TOUCH_Z_MIN 300
#define TOUCH_DEAD_SIZE 20
#define TOUCH_SQUARE_SIZE 10

namespace Touch
{
    TouchScreen::TouchScreen(TFT_eSPI *lcd)
    {
        this->lcd = lcd;
        lcd->resetViewport();
        this->screenHeight = lcd->height();
        this->screenWidth = lcd->width();
    }

    void TouchScreen::Calibrate()
    {
        lcd->resetViewport();
        lcd->fillScreen(TFT_BLACK);

        Point offsets[] = {
            // left up
            {TOUCH_DEAD_SIZE, TOUCH_DEAD_SIZE},
            // left down
            {TOUCH_DEAD_SIZE, this->screenHeight - TOUCH_SQUARE_SIZE - TOUCH_DEAD_SIZE},
            // right up
            {this->screenWidth - TOUCH_DEAD_SIZE - TOUCH_SQUARE_SIZE, TOUCH_DEAD_SIZE},
            // right down
            {this->screenWidth - TOUCH_DEAD_SIZE - TOUCH_SQUARE_SIZE, this->screenHeight - TOUCH_SQUARE_SIZE - TOUCH_DEAD_SIZE},
        };

        std::vector<Point> avg;
        const int touchCountMax = 10;

        lcd->setTextFont(1);
        lcd->setTextSize(2);

        for (int i = 0; i < 4; i++)
        {
            lcd->drawString(String("WAIT|"), 0, 0);
            for (int j = 0; j < 4; j++)
            {
                lcd->fillRect(offsets[j].x, offsets[j].y, TOUCH_SQUARE_SIZE, TOUCH_SQUARE_SIZE, TFT_RED);
            }

            delay(500);
            lcd->drawString(String("TOUCH|"), 0, 0);
            for (int j = 0; j < 4; j++)
            {
                lcd->fillRect(offsets[j].x, offsets[j].y, TOUCH_SQUARE_SIZE, TOUCH_SQUARE_SIZE, j == i ? TFT_GREEN : TFT_RED);
            }

            int touchCount = 0;
            std::vector<Point> points;

            while (touchCount < touchCountMax)
            {
                if (lcd->getTouchRawZ() > TOUCH_Z_MIN)
                {
                    uint16_t x;
                    uint16_t y;
                    lcd->getTouchRaw(&x, &y);

                    points.push_back(Point{x, y});
                    touchCount++;
                }
                delay(100);
            }

            lcd->drawString(String("UP|"), 0, 0);
            while (lcd->getTouchRawZ() > TOUCH_Z_MIN)
            {
                delay(50);
            }

            int avgX = 0;
            int avgY = 0;
            for (const auto &point : points)
            {
                avgX += point.x;
                avgY += point.y;
            }
            avg.push_back(Point{avgX / touchCountMax, avgY / touchCountMax});
        }

        this->config.isRotated = abs(avg[0].x - avg[1].x) > abs(avg[0].y - avg[1].y);
        if (this->config.isRotated)
        {
            for (int i = 0; i < 4; i++)
            {
                std::swap(avg[i].x, avg[i].y);
            }
        }

        this->config.minX = (avg[0].x + avg[1].x) / 2;
        this->config.maxX = (avg[2].x + avg[3].x) / 2;
        this->config.minY = (avg[0].y + avg[2].y) / 2;
        this->config.maxY = (avg[1].y + avg[3].y) / 2;

        this->config.isXInverse = this->config.minX > this->config.maxX;
        if (this->config.isXInverse)
        {
            std::swap(this->config.minX, this->config.maxX);
        }

        this->config.isYInverse = this->config.minY > this->config.maxY;
        if (this->config.isYInverse)
        {
            std::swap(this->config.minY, this->config.maxY);
        }

        lcd->drawString(String("CALIBRATED|"), 0, 0);
        lcd->fillScreen(TFT_BLACK);
    }

    const bool TouchScreen::GetTouch(Point &point) const
    {
        if (this->lcd->getTouchRawZ() < TOUCH_Z_MIN)
        {
            return false;
        }

        uint16_t rawX;
        uint16_t rawY;
        this->lcd->getTouchRaw(&rawX, &rawY);

        if (this->config.isRotated)
        {
            std::swap(rawX, rawY);
        }

        point.x = map(rawX, this->config.minX,
                      this->config.maxX,
                      TOUCH_DEAD_SIZE + TOUCH_SQUARE_SIZE / 2,
                      this->screenWidth - (TOUCH_DEAD_SIZE + TOUCH_SQUARE_SIZE / 2));

        point.y = map(rawY,
                      this->config.minY,
                      this->config.maxY,
                      TOUCH_DEAD_SIZE + TOUCH_SQUARE_SIZE / 2,
                      this->screenHeight - (TOUCH_DEAD_SIZE + TOUCH_SQUARE_SIZE / 2));

        if (this->config.isXInverse)
        {
            point.x = this->screenWidth - point.x;
        }

        if (this->config.isYInverse)
        {
            point.y = this->screenHeight - point.y;
        }

        return true;
    }

    void TouchScreen::SetConfig(const CalibrateConfig &config)
    {
        this->config = config;
    }

    const CalibrateConfig TouchScreen::GetConfig() const
    {
        return config;
    }
}
