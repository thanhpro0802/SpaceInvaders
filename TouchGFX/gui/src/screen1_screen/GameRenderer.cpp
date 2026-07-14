#include <gui/screen1_screen/GameRenderer.hpp>
#include <gui/screen1_screen/RetroGraphics.hpp>
#include <images/BitmapDatabase.hpp>
#include <touchgfx/Color.hpp>
#include <touchgfx/HAL/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <stdlib.h>
#include <string.h>

void GameRenderer::drawStartOverlay(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea) const
{
    touchgfx::Bitmap startPlanets(BITMAP_PLANETS_1_ID);
    int startPlanetH = startPlanets.getHeight();
    int startPlanetScrollY = (logic.backgroundY / 3) % startPlanetH;
    for (int y = -startPlanetH; y < 320; y += startPlanetH)
    {
        RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, -40, y + startPlanetScrollY, BITMAP_PLANETS_1_ID);
    }

    touchgfx::Rect shade(0, 0, 240, 320);
    touchgfx::Rect drawShade = shade & invalidatedArea;
    if (drawShade.width > 0 && drawShade.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawShade, touchgfx::Color::getColorFromRGB(0, 0, 0), 60);
    }

    touchgfx::colortype white = touchgfx::Color::getColorFromRGB(255, 255, 255);
    touchgfx::colortype black = touchgfx::Color::getColorFromRGB(0, 0, 0);
    RetroGraphics::drawOutlinedScaledString(absoluteRect, invalidatedArea, 8, 58, "SPACE INVADERS", 2, white, black);

    if (logic.showLeaderboard)
    {
        RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 80, 124, "TOP 5", 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
        for (int i = 0; i < 5; i++)
        {
            char row[24];
            char rank[4];
            char value[12];
            itoa(i + 1, rank, 10);
            itoa(logic.leaderboard[i], value, 10);
            strcpy(row, rank);
            strcat(row, ":");
            strcat(row, value);
            RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 72, 148 + i * 16, row, 2, touchgfx::Color::getColorFromRGB(220, 220, 220));
        }
    }
    else
    {
        float shipWave = sinf((float)logic.backgroundY * 0.035f);
        int shipX = (240 - logic.playerWidth) / 2 + (int)(shipWave * 30.0f);
        int shipY = 132 + (int)(sinf((float)logic.backgroundY * 0.022f) * 6.0f);
        int pFlameH = 10 + (rand() % 8);
        touchgfx::colortype flameCol = touchgfx::Color::getColorFromRGB(255, 100 + (rand() % 80), 0);
        for (int row = 0; row < pFlameH; row++)
        {
            int w = (pFlameH - row) * 8 / pFlameH;
            if (w > 0)
            {
                touchgfx::Rect rowRect(shipX + logic.playerWidth / 2 - w / 2, shipY + logic.playerHeight - 3 + row, w, 1);
                touchgfx::Rect drawRect = rowRect & invalidatedArea;
                if (drawRect.width > 0 && drawRect.height > 0)
                {
                    touchgfx::HAL::lcd().fillRect(drawRect, flameCol);
                }
            }
        }
        RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, shipX, shipY, BITMAP_PLAYER_SHIP_1_ID);
    }

    RetroGraphics::drawButton(absoluteRect, invalidatedArea, 54, 232, 132, 34, "PLAY", true);
    RetroGraphics::drawButton(absoluteRect, invalidatedArea, 18, 276, 204, 32, logic.showLeaderboard ? "BACK" : "LEADERBOARD", false);
}

void GameRenderer::drawGameOverOverlay(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea) const
{
    touchgfx::Rect shade(0, 0, 240, 320);
    touchgfx::Rect drawShade = shade & invalidatedArea;
    if (drawShade.width > 0 && drawShade.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawShade, touchgfx::Color::getColorFromRGB(0, 0, 0), 160);
    }

    RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 12, 80, "GAME OVER", 3, touchgfx::Color::getColorFromRGB(255, 255, 255));

    char scoreBuf[24] = "SCORE:";
    char scoreVal[12];
    itoa(logic.score, scoreVal, 10);
    strcat(scoreBuf, scoreVal);
    RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 36, 112, scoreBuf, 2, touchgfx::Color::getColorFromRGB(255, 255, 255));

    if (logic.showLeaderboard)
    {
        RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 80, 128, "TOP 5", 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
        for (int i = 0; i < 5; i++)
        {
            char row[24];
            char rank[4];
            char value[12];
            itoa(i + 1, rank, 10);
            itoa(logic.leaderboard[i], value, 10);
            strcpy(row, rank);
            strcat(row, ":");
            strcat(row, value);
            RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 72, 150 + i * 16, row, 2, touchgfx::Color::getColorFromRGB(220, 220, 220));
        }
    }
    else
    {
        RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 52, 152, "BEST", 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
        char bestVal[12];
        itoa(logic.highScore, bestVal, 10);
        RetroGraphics::drawScaledString(absoluteRect, invalidatedArea, 126, 152, bestVal, 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
    }

    RetroGraphics::drawButton(absoluteRect, invalidatedArea, 34, 232, 172, 28, "RESTART", true);
    RetroGraphics::drawButton(absoluteRect, invalidatedArea, 34, 266, 172, 22, logic.showLeaderboard ? "EXIT" : "TOP 5", false);
    RetroGraphics::drawButton(absoluteRect, invalidatedArea, 34, 294, 172, 22, "MENU", false);
}

void GameRenderer::drawBoss(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, bool isHitFlash) const
{
    touchgfx::Bitmap bmp(BITMAP_NAUTOLAN_SHIP_TORPEDO_SHIP_ID);
    int16_t w = bmp.getWidth(); // 64
    int16_t h = bmp.getHeight(); // 64
    const uint32_t* data32 = (const uint32_t*)bmp.getData();
    if (!data32) return;
    
    // Draw 2 glowing cyan/green engine exhausts at the top/rear of the Boss (flickering, aligned with 128x128 size)
    {
        int16_t exW = 8; // Làm cột lửa thon gọn hơn (rộng 8px)
        int16_t exH = 18 + (logic.gameTicks % 4) * 4; // Lập lòe độ dài
        
        touchgfx::colortype exCol1 = touchgfx::Color::getColorFromRGB(0, 255, 200);
        touchgfx::colortype exCol2 = touchgfx::Color::getColorFromRGB(80, 240, 255);
        touchgfx::colortype finalExCol = ((logic.gameTicks / 2) % 2 == 0) ? exCol1 : exCol2;
        
        // Cột lửa động cơ trái (kéo xuống y + 56 cho sát cánh boss, dịch vào x + 24)
        touchgfx::Rect leftEx(x + 24, y + 56 - exH, exW, exH);
        touchgfx::Rect drawLeftEx = leftEx & invalidatedArea;
        if (drawLeftEx.width > 0 && drawLeftEx.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawLeftEx, finalExCol);
        }
        
        // Cột lửa động cơ phải (kéo xuống y + 56 cho sát cánh boss, dịch vào x + 96 cho đối xứng hoàn hảo)
        touchgfx::Rect rightEx(x + 96, y + 56 - exH, exW, exH);
        touchgfx::Rect drawRightEx = rightEx & invalidatedArea;
        if (drawRightEx.width > 0 && drawRightEx.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawRightEx, finalExCol);
        }
    }
    
    // Vòng lặp quét từng pixel của ảnh gốc 64x64 để vẽ tỷ lệ 2x (128x128)
    for (int16_t by = 0; by < h; by++)
    {
        for (int16_t bx = 0; bx < w; bx++)
        {
            uint32_t pixel = data32[(h - 1 - by) * w + bx]; // Đảo chiều dọc (Y-flip) để mũi tàu Boss hướng xuống dưới
            uint8_t a = (pixel >> 24) & 0xFF;
            if (a > 10)
            {
                touchgfx::colortype col;
                if (isHitFlash)
                {
                    // Nhuộm đỏ 50%
                    uint8_t r_orig = (pixel >> 16) & 0xFF;
                    uint8_t g_orig = (pixel >> 8) & 0xFF;
                    uint8_t b_orig = pixel & 0xFF;
                    col = touchgfx::Color::getColorFromRGB((r_orig + 255) / 2, g_orig / 2, b_orig / 2);
                }
                else
                {
                    col = touchgfx::Color::getColorFromRGB((pixel >> 16) & 0xFF, (pixel >> 8) & 0xFF, pixel & 0xFF);
                }
                
                // Vẽ tỷ lệ 2x2 cho mỗi pixel
                touchgfx::Rect pixelBlock(x + bx * 2, y + by * 2, 2, 2);
                touchgfx::Rect drawBlock = pixelBlock & invalidatedArea;
                if (drawBlock.width > 0 && drawBlock.height > 0)
                {
                    touchgfx::HAL::lcd().fillRect(drawBlock, col, a);
                }
            }
        }
    }
}

void GameRenderer::render(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea) const
{
    // Determine which logic.level assets to draw based on transition state
    int drawLevel = ((logic.level - 1) % 3) + 1;
    if (logic.levelTransitionTimer > 60)
    {
        drawLevel = ((logic.level - 2) % 3) + 1;
        if (drawLevel < 1) drawLevel = 1;
    }

    uint16_t bgBmpId = BITMAP_BACKGROUND_1_ID;


    int startPlanetH = 320;
    int dustH = 320;
    int starH = 320;

    int startPlanetScrollY = (logic.backgroundY / 3) % startPlanetH;
    int dustScrollY = logic.backgroundY % dustH;
    int starScrollY = (logic.backgroundY / 2) % starH;

    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 0, startPlanetScrollY - startPlanetH, bgBmpId);
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 0, startPlanetScrollY, bgBmpId);
    
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 0, starScrollY - starH, BITMAP_STARS_1_ID);
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 0, starScrollY, BITMAP_STARS_1_ID);
    
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 0, dustScrollY - dustH, BITMAP_DUST_1_ID);
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 0, dustScrollY, BITMAP_DUST_1_ID);

    if (logic.screenState == STATE_START)
    {
        drawStartOverlay(logic, absoluteRect, invalidatedArea);
        return;
    }

    // Full-screen flash effect for Ultimate Bomb (drawn behind sprites)
    if (logic.flashScreenTimer > 0)
    {
        touchgfx::colortype flashCol = (logic.flashScreenTimer > 4) ? 
            touchgfx::Color::getColorFromRGB(255, 255, 255) : // White
            touchgfx::Color::getColorFromRGB(255, 120, 0);    // Orange
        
        touchgfx::Rect screenRect(0, 0, 240, 320);
        touchgfx::Rect drawFlash = screenRect & invalidatedArea;
        if (drawFlash.width > 0 && drawFlash.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawFlash, flashCol);
        }
    }

    // 2. Draw player flickering flame (Red/Orange)
    int pFlameH = 10 + (rand() % 8);
    touchgfx::colortype pFlameCol = touchgfx::Color::getColorFromRGB(255, 100 + (rand() % 80), 0);
    for (int row = 0; row < pFlameH; row++)
    {
        int w = (pFlameH - row) * 8 / pFlameH;
        if (w > 0)
        {
            touchgfx::Rect rowRect(logic.playerX + logic.playerWidth / 2 - w / 2, logic.playerY + logic.playerHeight - 3 + row, w, 1);
            touchgfx::Rect drawRect = rowRect & invalidatedArea;
            if (drawRect.width > 0 && drawRect.height > 0)
            {
                touchgfx::HAL::lcd().fillRect(drawRect, pFlameCol);
            }
        }
    }

    // 3. Draw barrier shield sprite if shield is active
    if (logic.hasShield)
    {
        int16_t bx = logic.playerX - (48 - logic.playerWidth) / 2;
        int16_t by = logic.playerY - (48 - logic.playerHeight) / 2;
        RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, bx, by, BITMAP_BARRIER_ID);
    }

    // 4. Draw player ship
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, logic.playerX, logic.playerY, BITMAP_PLAYER_SHIP_1_ID);
    if (logic.playerHitFlashTimer > 0)
    {
        RetroGraphics::drawStippleFlash(absoluteRect, invalidatedArea, logic.playerX, logic.playerY, BITMAP_PLAYER_SHIP_1_ID, touchgfx::Color::getColorFromRGB(0, 100, 255));
    }

    // 5. Draw logic.bullets
    for (int i = 0; i < GameLogic::MAX_BULLETS; i++)
    {
        if (logic.bullets[i].active)
        {
            if (logic.bullets[i].isBallBullet)
            {
                float pulse = sinf((float)logic.bullets[i].animFrame * 0.25f);
                int r = 4 + (int)(pulse * 1.5f);
                touchgfx::colortype outerCol = touchgfx::Color::getColorFromRGB(255, 170, 0);
                RetroGraphics::drawCircle(absoluteRect, invalidatedArea, (int16_t)logic.bullets[i].x, (int16_t)logic.bullets[i].y, r, outerCol);
                touchgfx::colortype innerCol = touchgfx::Color::getColorFromRGB(255, 255, 150);
                int ir = r / 2;
                if (ir > 0)
                {
                    RetroGraphics::drawCircle(absoluteRect, invalidatedArea, (int16_t)logic.bullets[i].x, (int16_t)logic.bullets[i].y, ir, innerCol);
                }
            }
            else
            {
                RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, (int16_t)logic.bullets[i].x, (int16_t)logic.bullets[i].y, BITMAP_SHOOT_1_ID);
            }
        }
    }

    // 6. Draw enemy engine flames
    for (int i = 0; i < GameLogic::MAX_ENEMIES; i++)
    {
        if (logic.enemies[i].active)
        {
            int eFlameH = 6 + (rand() % 6);
            touchgfx::colortype eFlameCol = touchgfx::Color::getColorFromRGB(0, 220 + (rand() % 35), 220);
            bool dualEngines = (logic.enemies[i].type == ENEMY_BALL_SHOOTER || logic.enemies[i].type == ENEMY_DIAGONAL_DASH);
            
            if (dualEngines)
            {
                int lx = (int)logic.enemies[i].x + logic.enemies[i].width / 4;
                for (int row = 0; row < eFlameH; row++)
                {
                    int w = (eFlameH - row) * 4 / eFlameH;
                    if (w > 0)
                    {
                        touchgfx::Rect rowRect(lx - w / 2, (int)logic.enemies[i].y + 14 - row, w, 1);
                        touchgfx::Rect drawRect = rowRect & invalidatedArea;
                        if (drawRect.width > 0 && drawRect.height > 0)
                        {
                            touchgfx::HAL::lcd().fillRect(drawRect, eFlameCol);
                        }
                    }
                }
                int rx = (int)logic.enemies[i].x + (logic.enemies[i].width * 3) / 4;
                for (int row = 0; row < eFlameH; row++)
                {
                    int w = (eFlameH - row) * 4 / eFlameH;
                    if (w > 0)
                    {
                        touchgfx::Rect rowRect(rx - w / 2, (int)logic.enemies[i].y + 14 - row, w, 1);
                        touchgfx::Rect drawRect = rowRect & invalidatedArea;
                        if (drawRect.width > 0 && drawRect.height > 0)
                        {
                            touchgfx::HAL::lcd().fillRect(drawRect, eFlameCol);
                        }
                    }
                }
            }
            else
            {
                int cx = (int)logic.enemies[i].x + logic.enemies[i].width / 2;
                for (int row = 0; row < eFlameH; row++)
                {
                    int w = (eFlameH - row) * 5 / eFlameH;
                    if (w > 0)
                    {
                        touchgfx::Rect rowRect(cx - w / 2, (int)logic.enemies[i].y + 14 - row, w, 1);
                        touchgfx::Rect drawRect = rowRect & invalidatedArea;
                        if (drawRect.width > 0 && drawRect.height > 0)
                        {
                            touchgfx::HAL::lcd().fillRect(drawRect, eFlameCol);
                        }
                    }
                }
            }
        }
    }

    // 7. Draw unique enemy ships
    for (int i = 0; i < GameLogic::MAX_ENEMIES; i++)
    {
        if (logic.enemies[i].active)
        {
            uint16_t enemyBmpId = BITMAP_ENEMY_SHIP_1_ID;
            switch (logic.enemies[i].type)
            {
            case ENEMY_TARGETED: enemyBmpId = BITMAP_ENEMY_SHIP_TARGETED_ID; break;
            case ENEMY_ZIGZAG: enemyBmpId = BITMAP_ENEMY_SHIP_ZIGZAG_ID; break;
            case ENEMY_HORIZONTAL: enemyBmpId = BITMAP_ENEMY_SHIP_HORIZONTAL_ID; break;
            case ENEMY_BURST_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_STRAIGHT_ID; break;
            case ENEMY_BALL_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_SPIRAL_ID; break;
            case ENEMY_DIAGONAL_DASH: enemyBmpId = BITMAP_ENEMY_SHIP_SHOOTER_ID; break;
            }
            RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, (int16_t)logic.enemies[i].x, (int16_t)logic.enemies[i].y, enemyBmpId);
            if (logic.enemies[i].hitFlashTimer > 0)
            {
                RetroGraphics::drawStippleFlash(absoluteRect, invalidatedArea, (int16_t)logic.enemies[i].x, (int16_t)logic.enemies[i].y, enemyBmpId, touchgfx::Color::getColorFromRGB(255, 0, 0));
            }
        }
    }

    // 7b. Draw Nautolan Torpedo Boss Ship
    if (logic.bossActive)
    {
        drawBoss(logic, absoluteRect, invalidatedArea, (int16_t)logic.bossX, (int16_t)logic.bossY, logic.bossHitFlashTimer > 0);
    }

    // 8. Draw logic.asteroids
    for (int i = 0; i < GameLogic::MAX_ASTEROIDS; i++)
    {
        if (logic.asteroids[i].active)
        {
            RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, (int16_t)logic.asteroids[i].x, (int16_t)logic.asteroids[i].y, logic.asteroids[i].bitmapId);
        }
    }

    // 9. Draw cropped item bonuses (using direct sprites!)
    for (int i = 0; i < GameLogic::MAX_ITEMS; i++)
    {
        if (logic.items[i].active)
        {
            uint16_t itemBmpId = BITMAP_ITEM_WEAPON_ID;
            switch (logic.items[i].type)
            {
            case ITEM_HEAL: itemBmpId = BITMAP_ITEM_HEAL_ID; break;
            case ITEM_SHIELD: itemBmpId = BITMAP_ITEM_SHIELD_ID; break;
            case ITEM_ULT: itemBmpId = BITMAP_ITEM_ULT_ID; break;
            case ITEM_WEAPON: itemBmpId = BITMAP_ITEM_WEAPON_ID; break;
            }
            RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, (int16_t)logic.items[i].x - 9, (int16_t)logic.items[i].y - 9, itemBmpId); // 18x18 centered
        }
    }

    // 10. Draw Sprite round explosion animations
    for (int i = 0; i < GameLogic::MAX_EXPLOSIONS; i++)
    {
        if (logic.explosions[i].active)
        {
            int frameIdx = logic.explosions[i].frame;
            if (frameIdx >= 0 && frameIdx < 10)
            {
                uint16_t expBmpId = BITMAP_EXP_ROUND_0_ID + frameIdx;
                touchgfx::Bitmap expBmp(expBmpId);
                int16_t ex = (int16_t)logic.explosions[i].x - expBmp.getWidth() / 2;
                int16_t ey = (int16_t)logic.explosions[i].y - expBmp.getHeight() / 2;
                RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, ex, ey, expBmpId);
            }
        }
    }

    // Draw Boss Health Bar at the bottom if Boss is Active
    if (logic.bossActive)
    {
        // Draw boss indicator text: "CHEFÃO" (using "CHEF AO" with custom tildes drawn on top)
        RetroGraphics::drawString(absoluteRect, invalidatedArea, 10, 305, "CHEF AO", touchgfx::Color::getColorFromRGB(255, 255, 255));
        
        // Draw tilde above A
        touchgfx::Rect tildeA(35, 303, 3, 1);
        touchgfx::Rect drawTildeA = tildeA & invalidatedArea;
        if (drawTildeA.width > 0 && drawTildeA.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawTildeA, touchgfx::Color::getColorFromRGB(255, 255, 255));
        }

        // Draw tilde above O
        touchgfx::Rect tildeO(41, 303, 3, 1);
        touchgfx::Rect drawTildeO = tildeO & invalidatedArea;
        if (drawTildeO.width > 0 && drawTildeO.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawTildeO, touchgfx::Color::getColorFromRGB(255, 255, 255));
        }

        // Dark red background bar on the right
        touchgfx::Rect bossHpBg(75, 305, 155, 8);
        touchgfx::Rect drawBossHpBg = bossHpBg & invalidatedArea;
        if (drawBossHpBg.width > 0 && drawBossHpBg.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawBossHpBg, touchgfx::Color::getColorFromRGB(60, 10, 10));
        }
        
        // Bright red foreground bar scaling with logic.bossHp / logic.bossMaxHp
        int bossHpW = (logic.bossHp * 155) / logic.bossMaxHp;
        if (bossHpW > 0)
        {
            touchgfx::Rect bossHpFg(75, 305, bossHpW, 8);
            touchgfx::Rect drawBossHpFg = bossHpFg & invalidatedArea;
            if (drawBossHpFg.width > 0 && drawBossHpFg.height > 0)
            {
                touchgfx::HAL::lcd().fillRect(drawBossHpFg, touchgfx::Color::getColorFromRGB(255, 10, 10));
            }
        }
    }



    // 11. Draw TOP HUD overlay bar (semi-transparent black bar)
    touchgfx::Rect hudBar(0, 0, 240, 32);
    touchgfx::Rect drawHudBar = hudBar & invalidatedArea;
    if (drawHudBar.width > 0 && drawHudBar.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawHudBar, touchgfx::Color::getColorFromRGB(5, 5, 15));
    }

    // Draw HUD text elements inside the bar
    // Draw player mini ship icon (16x16 image)
    RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 6, 2, BITMAP_PLAYER_SHIP_MINI_ID);
    char livesBuf[8];
    itoa(logic.lives, livesBuf, 10);
    RetroGraphics::drawString(absoluteRect, invalidatedArea, 26, 6, "X", touchgfx::Color::getColorFromRGB(255, 255, 255));
    RetroGraphics::drawString(absoluteRect, invalidatedArea, 34, 6, livesBuf, touchgfx::Color::getColorFromRGB(255, 255, 255));

    // Health Bar underneath logic.lives (Magenta bar)
    touchgfx::Rect hpBg(6, 18, 50, 4);
    touchgfx::Rect drawHpBg = hpBg & invalidatedArea;
    if (drawHpBg.width > 0 && drawHpBg.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawHpBg, touchgfx::Color::getColorFromRGB(50, 0, 20));
    }
    int hpW = (logic.health * 50) / 100;
    if (hpW > 0)
    {
        touchgfx::Rect hpFg(6, 18, hpW, 4);
        touchgfx::Rect drawHpFg = hpFg & invalidatedArea;
        if (drawHpFg.width > 0 && drawHpFg.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawHpFg, touchgfx::Color::getColorFromRGB(255, 0, 180));
        }
    }

    // B. Points / Score in the center
    char scoreBuf[32];
    char scoreVal[16];
    itoa(logic.score, scoreVal, 10);
    strcpy(scoreBuf, "SCORE:");
    strcat(scoreBuf, scoreVal);
    RetroGraphics::drawString(absoluteRect, invalidatedArea, 75, 4, scoreBuf, touchgfx::Color::getColorFromRGB(255, 255, 255));

    // Record / HighScore underneath logic.score
    char recordBuf[32];
    char recordVal[16];
    itoa(logic.highScore, recordVal, 10);
    strcpy(recordBuf, "BEST:");
    strcat(recordBuf, recordVal);
    RetroGraphics::drawString(absoluteRect, invalidatedArea, 75, 16, recordBuf, touchgfx::Color::getColorFromRGB(180, 180, 180));

    // C. Difficulty logic.level, Ultimate and Timer on the Right
    int totalSec = logic.gameTicks / 60;
    int min = totalSec / 60;
    int sec = totalSec % 60;
    int centis = (logic.gameTicks % 60) * 100 / 60;

    char timeBuf[16];
    char tmp[8];
    if (min < 10) strcpy(timeBuf, "0");
    else timeBuf[0] = '\0';
    itoa(min, tmp, 10);
    strcat(timeBuf, tmp);
    strcat(timeBuf, ":");
    if (sec < 10) strcat(timeBuf, "0");
    itoa(sec, tmp, 10);
    strcat(timeBuf, tmp);
    strcat(timeBuf, ":");
    if (centis < 10) strcat(timeBuf, "0");
    itoa(centis, tmp, 10);
    strcat(timeBuf, tmp);

    RetroGraphics::drawString(absoluteRect, invalidatedArea, 178, 4, timeBuf, touchgfx::Color::getColorFromRGB(255, 255, 100));

    // Difficulty Level (LV) based on logic.level variable
    char lvBuf[16] = "LV";
    itoa(logic.level, tmp, 10);
    strcat(lvBuf, tmp);
    RetroGraphics::drawString(absoluteRect, invalidatedArea, 178, 16, lvBuf, touchgfx::Color::getColorFromRGB(100, 255, 255));

    // Draw mini ultimate icons next to LV
    for (int u = 0; u < logic.ultCount; u++)
    {
        RetroGraphics::drawBitmap(absoluteRect, invalidatedArea, 204 + u * 12, 16, BITMAP_ITEM_ULT_MINI_ID);
    }

    // Draw Level Transition text banner (centered)
    if (logic.levelTransitionTimer > 0)
    {
        // Dark translucent overlay box in the center
        touchgfx::Rect box(20, 130, 200, 40);
        touchgfx::Rect drawBox = box & invalidatedArea;
        if (drawBox.width > 0 && drawBox.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawBox, touchgfx::Color::getColorFromRGB(10, 10, 25));
        }

        // Draw transition text
        if (logic.levelTransitionTimer > 60)
        {
            RetroGraphics::drawString(absoluteRect, invalidatedArea, 56, 146, "HYPERJUMP...", touchgfx::Color::getColorFromRGB(255, 255, 100));
        }
        else
        {
            char stageText[32] = "STAGE ";
            char num[8];
            itoa(logic.level, num, 10);
            strcat(stageText, num);
            strcat(stageText, " START");
            RetroGraphics::drawString(absoluteRect, invalidatedArea, 68, 146, stageText, touchgfx::Color::getColorFromRGB(100, 255, 100));
        }
    }

    if (logic.screenState == STATE_GAME_OVER)
    {
        drawGameOverOverlay(logic, absoluteRect, invalidatedArea);
    }
}
