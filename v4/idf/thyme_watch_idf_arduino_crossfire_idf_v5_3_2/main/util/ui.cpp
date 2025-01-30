#include "ui.h"

void printCenteredText(Arduino_Canvas_6bit* gfx, const char *text, uint8_t color, uint8_t size, uint16_t centerX, uint16_t centerY)
{
    gfx->setFont((const GFXfont *)nullptr);
    // canvas->setTextColor(color);
    // canvas->setTextSize(size);
    uint16_t width, height;
    int16_t x1, y1;
    // canvas->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
    // canvas->setCursor(centerX - width / 2, centerY - height / 2);
    // canvas->print(text);

    gfx->setTextSize(size);
    // gfx->setFont(nullptr);
    gfx->setTextColor(RGB565_WHITE);
    gfx->getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
    gfx->setCursor(centerX - width / 2, centerY - height / 2);
    gfx->print(text);
}
