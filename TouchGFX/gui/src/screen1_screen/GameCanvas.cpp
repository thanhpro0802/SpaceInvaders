#include <gui/screen1_screen/GameCanvas.hpp>
#include <touchgfx/HAL/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <gui/screen1_screen/RetroGraphics.hpp>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Retro Gaming style 7x7 bitmap font. Each row uses bits 6..0 from left to right.


GameCanvas::GameCanvas()
{
    // Enable touch input
    setTouchable(true);

    logic.playerWidth = touchgfx::Bitmap(BITMAP_PLAYER_SHIP_1_ID).getWidth();
    logic.playerHeight = touchgfx::Bitmap(BITMAP_PLAYER_SHIP_1_ID).getHeight();
    logic.playerSpeed = 5;

    logic.highScore = 18000; // default initial high logic.score
    for (int i = 0; i < 5; i++)
    {
        logic.leaderboard[i] = 0;
    }
    logic.leaderboard[0] = logic.highScore;
    logic.screenState = STATE_PLAYING;
    logic.showLeaderboard = false;

    logic.resetGame();
    logic.screenState = STATE_START;
}


touchgfx::Rect GameCanvas::getSolidRect() const
{
    return touchgfx::Rect(0, 0, getWidth(), getHeight());
}















void GameCanvas::handleClickEvent(const touchgfx::ClickEvent& event)
{
    if (event.getType() == touchgfx::ClickEvent::PRESSED)
    {
        if (logic.screenState == STATE_START)
        {
            int x = event.getX();
            int y = event.getY();
            if (x >= 54 && x <= 186 && y >= 232 && y <= 266)
            {
                logic.resetGame();
            }
            else if (x >= 18 && x <= 222 && y >= 276 && y <= 308)
            {
                logic.showLeaderboard = !logic.showLeaderboard;
            }
            invalidate();
            return;
        }

        if (logic.screenState == STATE_GAME_OVER)
        {
            int x = event.getX();
            int y = event.getY();
            if (x >= 34 && x <= 206 && y >= 232 && y <= 260)
            {
                logic.resetGame();
            }
            else if (x >= 34 && x <= 206 && y >= 266 && y <= 288)
            {
                logic.showLeaderboard = !logic.showLeaderboard;
            }
            else if (x >= 34 && x <= 206 && y >= 294 && y <= 316)
            {
                logic.resetGame();
                logic.screenState = STATE_START;
                logic.showLeaderboard = false;
            }
            invalidate();
            return;
        }

        logic.playerX = event.getX() - logic.playerWidth / 2;
        logic.playerY = event.getY() - logic.playerHeight / 2;
        invalidate();
    }
}

void GameCanvas::handleDragEvent(const touchgfx::DragEvent& event)
{
    if (logic.screenState == STATE_START)
    {
        return;
    }

    if (logic.screenState == STATE_GAME_OVER)
    {
        return;
    }

    logic.playerX = event.getNewX() - logic.playerWidth / 2;
    logic.playerY = event.getNewY() - logic.playerHeight / 2;

    // Bounds checking
    if (logic.playerX < 0) logic.playerX = 0;
    if (logic.playerX > 240 - logic.playerWidth) logic.playerX = 240 - logic.playerWidth;
    if (logic.playerY < 32) logic.playerY = 32;
    if (logic.playerY > 320 - logic.playerHeight) logic.playerY = 320 - logic.playerHeight;

    invalidate();
}

void GameCanvas::update()
{
    logic.update();
    invalidate();
}








void GameCanvas::handleKeyEvent(uint8_t key)
{
    logic.handleKeyEvent(key);
}

void GameCanvas::draw(const touchgfx::Rect& invalidatedArea) const
{
    renderer.render(logic, getAbsoluteRect(), invalidatedArea);
}
