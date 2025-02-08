#include "MusicPlayer.h"
#include <common.h>
#include <config.h>
#include <hal/hal_ls012b7dd06.h>

#include <RasterGothic18CondBold18pt7b.h>

#include "util/ui.h"
#include <apps/apps.h>
#include "framework/app_manager.h"
#include "resources/music_player.h"

#define TAG "MusicPlayer"

using namespace Thyme;

MusicPlayer::MusicPlayer(void* params) : ThymeApp(params)
{
    MY_LOG("MusicPlayer::MusicPlayer()");
}

std::string MusicPlayer::appId()
{
    return "music";
}

void MusicPlayer::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("MusicPlayer::onStart()");
}

void MusicPlayer::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("MusicPlayer::onStop()");
}

void MusicPlayer::onDraw(Arduino_Canvas_6bit *gfx)
{
    gfx->fillScreen(RGB565_BLACK);
    memcpy(gfx->getFramebuffer(), music_player, 240 * 240);
    printCenteredText(gfx, "Template App", RGB565_WHITE, 1, 120, 120);
    flushDisplay(gfx->getFramebuffer());
}

void MusicPlayer::onUpButtonPressed()
{
    MY_LOG("MusicPlayer::onUpButtonPressed()");
}

void MusicPlayer::onMiddleButtonPressed()
{
    MY_LOG("MusicPlayer::onMiddleButtonPressed()");
}

void MusicPlayer::onDownButtonPressed()
{
    MY_LOG("MusicPlayer::onDownButtonPressed()");
}

MusicPlayer::~MusicPlayer()
{
    MY_LOG("MusicPlayer::~MusicPlayer()");
}
