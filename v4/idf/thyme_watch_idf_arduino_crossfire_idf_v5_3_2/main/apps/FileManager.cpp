#include "FileManager.h"
#include <common.h>
#include <config.h>
#include <hal/hal_ls012b7dd06.h>

#include <RasterGothic18CondBold18pt7b.h>

#include "util/ui.h"
#include "FS.h"
#include <apps/apps.h>
#include "framework/app_manager.h"

#define TAG "FileManager"

using namespace Thyme;

FileManager::FileManager(void *params) : ThymeApp(params)
{
    MY_LOG("FileManager::FileManager()");
}

std::string FileManager::appId()
{
    return "file_manager";
}

void FileManager::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("FileManager::onStart()");
    m_currentFiles = listDir("/");
    for (auto file : m_currentFiles)
    {
        MY_LOG("  %s: %s  SIZE: %u", file.isDirectory ? "DIR" : "FILE",
               file.name.c_str(), file.size);
    }
}

void FileManager::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("FileManager::onStop()");
}

void FileManager::onDraw(Arduino_Canvas_6bit *gfx)
{
    gfx->fillScreen(RGB565_BLACK);
    gfx->setTextColor(RGB565_WHITE);
    gfx->setUTF8Print(true);
    gfx->setFont(u8g2_font_wqy12_t_gb2312);
    const int16_t fontHeight = 12;
    for (size_t i = 0; i < m_currentFiles.size(); i++)
    {
        auto file = m_currentFiles[i];
        gfx->setCursor(0, fontHeight * i);
        // print dir or file prefix
        if (file.isDirectory) {
            gfx->print("D ");
        } else {
            gfx->print("F ");
        }
        gfx->print(file.name.c_str());
    }
    // printCenteredText(gfx, "Template App", RGB565_WHITE, 1, 120, 120);
    flushDisplay(gfx->getFramebuffer());
}

void FileManager::onUpButtonPressed()
{
    MY_LOG("FileManager::onUpButtonPressed()");
}

void FileManager::onMiddleButtonPressed()
{
    MY_LOG("FileManager::onMiddleButtonPressed()");
}

void FileManager::onDownButtonPressed()
{
    MY_LOG("FileManager::onDownButtonPressed()");
}

FileManager::~FileManager()
{
    MY_LOG("FileManager::~FileManager()");
}
