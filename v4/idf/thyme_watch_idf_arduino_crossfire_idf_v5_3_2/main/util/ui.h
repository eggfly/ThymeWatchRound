#pragma once

#include <Arduino_GFX_Library.h>
#include "Arduino_Canvas_6bit.h"

void printCenteredText(Arduino_Canvas_6bit *gfx, const char *text, uint8_t color, uint8_t size, uint16_t centerX, uint16_t centerY);
