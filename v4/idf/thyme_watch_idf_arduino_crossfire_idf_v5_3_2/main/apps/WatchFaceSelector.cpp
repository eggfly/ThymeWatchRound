#include "WatchFaceSelector.h"

#include <common.h>
#include <config.h>
#include "hal/hal_ls012b7dd06.h"
#include "hal/hal_rtc.h"
#include <RasterGothic18CondBold18pt7b.h>

#include "hal/hal_ls012b7dd06.h"
#include "util/ui.h"

#include <resources/anim_data_12_frames.h>

#define TAG "WatchFaceSelector"

WatchFaceSelector::WatchFaceSelector(void *params)
    : ThymeApp(params)
{
    MY_LOG("WatchFaceSelector::WatchFaceSelector()");
}

std::string WatchFaceSelector::appId()
{
    return "watch_face_selector";
}

void WatchFaceSelector::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("WatchFaceSelector::onStart()");
}

void WatchFaceSelector::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("WatchFaceSelector::onStop()");
}

void WatchFaceSelector::onDraw(Arduino_Canvas_6bit *gfx)
{
    renderNyanCat(gfx);
    flushDisplay(gfx->getFramebuffer());
}

WatchFaceSelector::~WatchFaceSelector()
{
    MY_LOG("WatchFaceSelector::~WatchFaceSelector()");
}

void WatchFaceSelector::renderNyanCat(Arduino_Canvas_6bit *gfx)
{
    uint16_t nyanCatFrameCount = sizeof(anim_data) / sizeof(anim_data[0]);
    MY_LOG("nyanCatFrameCount: %d", nyanCatFrameCount);
    memcpy(gfx->getFramebuffer(), anim_data[nyanCatFramePos], 240 * 240);
    nyanCatFramePos++;
    if (nyanCatFramePos >= nyanCatFrameCount)
    {
        nyanCatFramePos = 0;
    }
}
