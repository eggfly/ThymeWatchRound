#include "TemplateApp.h"
#include <common.h>
#include <config.h>
#include <hal/hal_ls012b7dd06.h>

#include <RasterGothic18CondBold18pt7b.h>

#include "util/ui.h"
#include <apps/apps.h>
#include "framework/app_manager.h"

#define TAG "TemplateApp"

using namespace Thyme;

TemplateApp::TemplateApp()
{
    MY_LOG("TemplateApp::TemplateApp()");
}

std::string TemplateApp::appId()
{
    return "template_app";
}

void TemplateApp::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("TemplateApp::onStart()");
}

void TemplateApp::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("TemplateApp::onStop()");
}

void TemplateApp::onDraw(Arduino_Canvas_6bit *gfx)
{
    gfx->fillScreen(RGB565_BLACK);
    printCenteredText(gfx, "Template App", RGB565_WHITE, 1, 120, 120);
    flushDisplay(gfx->getFramebuffer());
}

void TemplateApp::onUpButtonPressed()
{
    MY_LOG("TemplateApp::onUpButtonPressed()");
}

void TemplateApp::onMiddleButtonPressed()
{
    MY_LOG("TemplateApp::onMiddleButtonPressed()");
}

void TemplateApp::onDownButtonPressed()
{
    MY_LOG("TemplateApp::onDownButtonPressed()");
}

TemplateApp::~TemplateApp()
{
    MY_LOG("TemplateApp::~TemplateApp()");
}
