#include "Game_Breakout.h"
#include <common.h>
#include <config.h>
#include <hal/hal_ls012b7dd06.h>

#include <RasterGothic18CondBold18pt7b.h>

#include "util/ui.h"
#include <apps/apps.h>
#include "framework/app_manager.h"

#define TAG "Game_Breakout"

using namespace Thyme;

Game_Breakout::Game_Breakout()
{
    MY_LOG("Game_Breakout::Game_Breakout()");
}

std::string Game_Breakout::appId()
{
    return "breakout_game";
}

void Game_Breakout::onStart(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("Game_Breakout::onStart()");
}

void Game_Breakout::onStop(Arduino_Canvas_6bit *gfx)
{
    MY_LOG("Game_Breakout::onStop()");
}

void Game_Breakout::onDraw(Arduino_Canvas_6bit *gfx)
{
    unsigned long time = millis();
    gfx->fillScreen(RGB565_BLACK);
    printCenteredText(gfx, "BREAKOUT", RGB565_WHITE, 1, 120, 10);
    gfx->drawRect(120 - WIDTH_HEIGHT / 2, 120 - WIDTH_HEIGHT / 2, WIDTH_HEIGHT, WIDTH_HEIGHT, RGB565_WHITE);
    uint16_t breakoutMargin = 2;
    // Draw bricks on top
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            uint8_t randomRed= rand() & 0xff;
            uint8_t randomGreen = rand() & 0xff;
            uint8_t randomBlue = rand() & 0xff;
            uint16_t randomColor = RGB565(randomRed, randomGreen, randomBlue);
            // draw 4 lines of bricks, with margin 2, with random colors
            gfx->fillRect(120 - WIDTH_HEIGHT / 2 + i * (WIDTH_HEIGHT / 10) + breakoutMargin,
                          120 - WIDTH_HEIGHT / 2 + j * (WIDTH_HEIGHT / 20) + breakoutMargin,
                          WIDTH_HEIGHT / 10 - 2 * breakoutMargin,
                          WIDTH_HEIGHT / 20 - 2 * breakoutMargin,
                          randomColor);
        }
    }
    // Draw paddle with watch's digital crown position as x
    int16_t paddleWidth = 40;
    int16_t paddleHeight = 5;
    uint16_t pos = constrain(lastDigitalCrownPosition, -WIDTH_HEIGHT / 2 + paddleWidth / 2, WIDTH_HEIGHT / 2 - paddleWidth / 2);
    int16_t paddleX = pos + LCD_WIDTH / 2 - paddleWidth / 2;
    int16_t paddleBottomPadding = 2;
    int16_t paddleY = 120 + WIDTH_HEIGHT / 2 - paddleHeight - paddleBottomPadding;
    gfx->fillRoundRect(paddleX, paddleY, paddleWidth, paddleHeight, 2, RGB565_WHITE);
    // update ball position
    ballPosX += ballSpeedX * (time - lastTime) / 1000;
    ballPosY += ballSpeedY * (time - lastTime) / 1000;

    // 碰撞检测: 碰壁
    if (ballPosX < 0 || ballPosX > WIDTH_HEIGHT)
    {
        ballSpeedX = -ballSpeedX;
    }
    if (ballPosY < 0 || ballPosY > WIDTH_HEIGHT)
    {
        ballSpeedY = -ballSpeedY;
    }
    // 碰撞检测: 碰到挡板
    if (ballPosY + 2 >= paddleY && ballPosY <= paddleY + paddleHeight)
    {
        if (ballPosX + 2 >= paddleX && ballPosX <= paddleX + paddleWidth)
        {
            ballSpeedY = -ballSpeedY;
        }
    }
    // 绘制小球
    gfx->fillCircle(ballPosX, ballPosY, BALL_RADIUS, RGB565_CYAN);
    MY_LOG("ballPosX: %f, ballPosY: %f", ballPosX, ballPosY);
    flushDisplay(gfx->getFramebuffer());
    lastTime = time;
}

void Game_Breakout::onDigitalCrownRotated(long position)
{
    lastDigitalCrownPosition = position;
}

void Game_Breakout::onUpButtonPressed()
{
    MY_LOG("Game_Breakout::onUpButtonPressed()");
}

void Game_Breakout::onMiddleButtonPressed()
{
    MY_LOG("Game_Breakout::onMiddleButtonPressed()");
}

void Game_Breakout::onDownButtonPressed()
{
    MY_LOG("Game_Breakout::onDownButtonPressed()");
}

Game_Breakout::~Game_Breakout()
{
    MY_LOG("Game_Breakout::~Game_Breakout()");
}
