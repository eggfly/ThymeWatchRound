#include "SensorsApp.h"
#include <common.h>
#include <config.h>
#include <hal/hal_ls012b7dd06.h>

#include <RasterGothic18CondBold18pt7b.h>

#include "util/ui.h"
#include <apps/apps.h>
#include "framework/app_manager.h"

#define TAG "SensorsApp"

using namespace Thyme;

SensorsApp::SensorsApp()
{
    MY_LOG("SensorsApp::SensorsApp()");
}

std::string SensorsApp::appId()
{
    return "template_app";
}

void SensorsApp::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("SensorsApp::onStart()");
}

void SensorsApp::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("SensorsApp::onStop()");
}

void SensorsApp::onDraw(Arduino_Canvas_6bit *gfx)
{
    gfx->fillScreen(RGB565_BLACK);
    printCenteredText(gfx, "Template App", RGB565_WHITE, 1, 120, 120);
    flushDisplay(gfx->getFramebuffer());
}

void SensorsApp::onUpButtonPressed()
{
    MY_LOG("SensorsApp::onUpButtonPressed()");
}

void SensorsApp::onMiddleButtonPressed()
{
    MY_LOG("SensorsApp::onMiddleButtonPressed()");
}

void SensorsApp::onDownButtonPressed()
{
    MY_LOG("SensorsApp::onDownButtonPressed()");
}

SensorsApp::~SensorsApp()
{
    MY_LOG("SensorsApp::~SensorsApp()");
}
