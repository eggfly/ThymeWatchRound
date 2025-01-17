#include "ThymeWatchFace.h"
#include <common.h>
#include "hal/hal_ls012b7dd06.h"
#include <RasterGothic18CondBold18pt7b.h>

#define TAG "ThymeWatchFace"


ThymeWatchFace::ThymeWatchFace()
{
    MY_LOG("ThymeWatchFace::ThymeWatchFace()");
}

std::string ThymeWatchFace::appId()
{
    return "watch_face";
}

void ThymeWatchFace::onStart()
{
    MY_LOG("ThymeWatchFace::onStart()");
}

void ThymeWatchFace::onStop()
{
    MY_LOG("ThymeWatchFace::onStop()");
}


void ThymeWatchFace::onDraw(Arduino_Canvas_6bit *gfx)
{
    
    flushDisplay(gfx->getFramebuffer());
}

void ThymeWatchFace::onButtonEvent(uint8_t buttonState, uint8_t prevState)
{
    MY_LOG("ThymeWatchMenu::onButtonEvent(%d, %d)", buttonState, prevState);
    bool btnUp = buttonState & 0b001;
    bool btnMid = buttonState & 0b010;
    bool btnDown = buttonState & 0b100;
    bool btnUpPrev = prevState & 0b001;
    bool btnMidPrev = prevState & 0b010;
    bool btnDownPrev = prevState & 0b100;
    if (btnUp && !btnUpPrev)
    {
        MY_LOG("Up pressed");
    }
    if (btnMid && !btnMidPrev)
    {
        MY_LOG("Mid pressed");
    }
    if (btnDown && !btnDownPrev)
    {
        MY_LOG("Down pressed");
    }
}

ThymeWatchFace::~ThymeWatchFace()
{
    MY_LOG("ThymeWatchFace::~ThymeWatchFace()");
}

void ThymeWatchFace::onBackPressed()
{
    MY_LOG("ThymeWatchFace::onBackPressed()");
}
