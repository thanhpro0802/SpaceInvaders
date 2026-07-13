#include <gui/screen1_screen/GameCanvas.hpp>
#include <touchgfx/HAL/HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Retro Gaming style 7x7 bitmap font. Each row uses bits 6..0 from left to right.
static const uint8_t retroFont7x7[36][7] = {
    {0x3E, 0x63, 0x67, 0x6B, 0x73, 0x63, 0x3E}, // 0
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E}, // 1
    {0x3E, 0x63, 0x03, 0x0E, 0x38, 0x60, 0x7F}, // 2
    {0x3E, 0x63, 0x03, 0x1E, 0x03, 0x63, 0x3E}, // 3
    {0x0E, 0x1E, 0x36, 0x66, 0x7F, 0x06, 0x06}, // 4
    {0x7F, 0x60, 0x7E, 0x03, 0x03, 0x63, 0x3E}, // 5
    {0x1E, 0x30, 0x60, 0x7E, 0x63, 0x63, 0x3E}, // 6
    {0x7F, 0x63, 0x06, 0x0C, 0x18, 0x18, 0x18}, // 7
    {0x3E, 0x63, 0x63, 0x3E, 0x63, 0x63, 0x3E}, // 8
    {0x3E, 0x63, 0x63, 0x3F, 0x03, 0x06, 0x3C}, // 9
    {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00}, // :
    {0x7E, 0x63, 0x63, 0x7E, 0x60, 0x60, 0x60}, // P
    {0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E}, // O
    {0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63}, // N
    {0x7F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // T
    {0x3F, 0x60, 0x60, 0x3E, 0x03, 0x03, 0x7E}, // S
    {0x7E, 0x63, 0x63, 0x7E, 0x6C, 0x66, 0x63}, // R
    {0x7F, 0x60, 0x60, 0x7E, 0x60, 0x60, 0x7F}, // E
    {0x3E, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3E}, // C
    {0x7C, 0x66, 0x63, 0x63, 0x63, 0x66, 0x7C}, // D
    {0x63, 0x63, 0x63, 0x36, 0x36, 0x1C, 0x08}, // V
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7F}, // L
    {0x63, 0x36, 0x1C, 0x08, 0x1C, 0x36, 0x63}, // X
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00}, // -
    {0x63, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x63}, // H
    {0x63, 0x63, 0x36, 0x1C, 0x18, 0x18, 0x18}, // Y
    {0x1F, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C}, // J
    {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E}, // U
    {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63}, // M
    {0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63}, // A
    {0x3E, 0x63, 0x60, 0x6F, 0x63, 0x63, 0x3E}, // G
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18}, // .
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E}, // I
    {0x7E, 0x63, 0x63, 0x7E, 0x63, 0x63, 0x7E}, // B
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // space
};

static int getFontIndex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c == ':') return 10;
    if (c == 'P' || c == 'p') return 11;
    if (c == 'O' || c == 'o') return 12;
    if (c == 'N' || c == 'n') return 13;
    if (c == 'T' || c == 't') return 14;
    if (c == 'S' || c == 's') return 15;
    if (c == 'R' || c == 'r') return 16;
    if (c == 'E' || c == 'e') return 17;
    if (c == 'C' || c == 'c') return 18;
    if (c == 'D' || c == 'd') return 19;
    if (c == 'V' || c == 'v') return 20;
    if (c == 'L' || c == 'l') return 21;
    if (c == 'X' || c == 'x') return 22;
    if (c == '-') return 23;
    if (c == 'H' || c == 'h') return 24;
    if (c == 'Y' || c == 'y') return 25;
    if (c == 'J' || c == 'j') return 26;
    if (c == 'U' || c == 'u') return 27;
    if (c == 'M' || c == 'm') return 28;
    if (c == 'A' || c == 'a') return 29;
    if (c == 'G' || c == 'g') return 30;
    if (c == '.') return 31;
    if (c == 'I' || c == 'i') return 32;
    if (c == 'B' || c == 'b') return 33;
    return 34; // space
}

GameCanvas::GameCanvas()
{
    // Enable touch input
    setTouchable(true);

    playerWidth = touchgfx::Bitmap(BITMAP_PLAYER_SHIP_1_ID).getWidth();
    playerHeight = touchgfx::Bitmap(BITMAP_PLAYER_SHIP_1_ID).getHeight();
    playerSpeed = 5;

    highScore = 18000; // default initial high score
    for (int i = 0; i < 5; i++)
    {
        leaderboard[i] = 0;
    }
    leaderboard[0] = highScore;
    screenState = STATE_PLAYING;
    showLeaderboard = false;

    resetGame();
    screenState = STATE_START;
}

void GameCanvas::resetGame()
{
    playerX = (240 - playerWidth) / 2;
    playerY = 320 - playerHeight - 30;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        asteroids[i].active = false;
    }

    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        explosions[i].active = false;
    }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        items[i].active = false;
    }

    backgroundY = 0;
    enemySpawnTimer = 0;
    asteroidSpawnTimer = 0;
    shootCooldownTimer = 0;

    // HUD Init
    score = 0;
    lives = 3;
    health = 100;
    weaponLevel = 1;
    gameTicks = 0;
    hasShield = false;
    ultCount = 0;
    flashScreenTimer = 0;
    level = 1;
    levelTransitionTimer = 0;
    playerHitFlashTimer = 0;
    screenState = STATE_PLAYING;
    showLeaderboard = false;
    
    isBossFight = false;
    bossActive = false;
    bossHp = 0;
    bossMaxHp = 0;
    bossHitFlashTimer = 0;
    bossWidth = 48; // will be updated from bitmap once loaded
    bossHeight = 48;
    victoryTimer = 0;
}

touchgfx::Rect GameCanvas::getSolidRect() const
{
    return touchgfx::Rect(0, 0, getWidth(), getHeight());
}

void GameCanvas::drawBitmap(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, uint16_t bitmapId) const
{
    touchgfx::Bitmap bmp(bitmapId);
    touchgfx::Rect bmpRect(x, y, bmp.getWidth(), bmp.getHeight());
    touchgfx::Rect dirty = bmpRect & invalidatedArea;
    if (dirty.width > 0 && dirty.height > 0)
    {
        int16_t absoluteX = getAbsoluteRect().x + x;
        int16_t absoluteY = getAbsoluteRect().y + y;
        
        touchgfx::Rect bitmapArea = dirty;
        bitmapArea.x -= x;
        bitmapArea.y -= y;
        
        touchgfx::HAL::lcd().drawPartialBitmap(bmp, absoluteX, absoluteY, bitmapArea, 255);
    }
}

void GameCanvas::drawCircle(const touchgfx::Rect& invalidatedArea, int16_t cx, int16_t cy, int r, touchgfx::colortype col) const
{
    for (int dy = -r; dy <= r; dy++)
    {
        int dx = (int)sqrtf((float)(r * r - dy * dy));
        if (dx > 0)
        {
            touchgfx::Rect rowRect(cx - dx, cy + dy, dx * 2, 1);
            touchgfx::Rect drawRect = rowRect & invalidatedArea;
            if (drawRect.width > 0 && drawRect.height > 0)
            {
                touchgfx::HAL::lcd().fillRect(drawRect, col);
            }
        }
    }
}

void GameCanvas::drawChar(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, char c, touchgfx::colortype col) const
{
    int idx = getFontIndex(c);
    for (int rowIdx = 0; rowIdx < 7; rowIdx++)
    {
        uint8_t row = retroFont7x7[idx][rowIdx];
        for (int colIdx = 0; colIdx < 7; colIdx++)
        {
            if (row & (1 << (6 - colIdx)))
            {
                touchgfx::Rect pixelRect(x + colIdx, y + rowIdx, 1, 1);
                touchgfx::Rect drawRect = pixelRect & invalidatedArea;
                if (drawRect.width > 0 && drawRect.height > 0)
                {
                    touchgfx::HAL::lcd().fillRect(drawRect, col);
                }
            }
        }
    }
}

void GameCanvas::drawString(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, touchgfx::colortype col) const
{
    int16_t cx = x;
    while (*str)
    {
        drawChar(invalidatedArea, cx, y, *str, col);
        cx += 8; // 7 pixels width + 1 pixel spacing
        str++;
    }
}

void GameCanvas::drawScaledChar(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, char c, int scale, touchgfx::colortype col) const
{
    int idx = getFontIndex(c);
    for (int rowIdx = 0; rowIdx < 7; rowIdx++)
    {
        uint8_t row = retroFont7x7[idx][rowIdx];
        for (int colIdx = 0; colIdx < 7; colIdx++)
        {
            if (row & (1 << (6 - colIdx)))
            {
                touchgfx::Rect pixelRect(x + colIdx * scale, y + rowIdx * scale, scale, scale);
                touchgfx::Rect drawRect = pixelRect & invalidatedArea;
                if (drawRect.width > 0 && drawRect.height > 0)
                {
                    touchgfx::HAL::lcd().fillRect(drawRect, col);
                }
            }
        }
    }
}

void GameCanvas::drawScaledString(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, int scale, touchgfx::colortype col) const
{
    int16_t cx = x;
    while (*str)
    {
        drawScaledChar(invalidatedArea, cx, y, *str, scale, col);
        cx += 8 * scale;
        str++;
    }
}

void GameCanvas::drawOutlinedScaledString(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, int scale, touchgfx::colortype fill, touchgfx::colortype outline) const
{
    drawScaledString(invalidatedArea, x - scale, y, str, scale, outline);
    drawScaledString(invalidatedArea, x + scale, y, str, scale, outline);
    drawScaledString(invalidatedArea, x, y - scale, str, scale, outline);
    drawScaledString(invalidatedArea, x, y + scale, str, scale, outline);
    drawScaledString(invalidatedArea, x - scale, y - scale, str, scale, outline);
    drawScaledString(invalidatedArea, x + scale, y - scale, str, scale, outline);
    drawScaledString(invalidatedArea, x - scale, y + scale, str, scale, outline);
    drawScaledString(invalidatedArea, x + scale, y + scale, str, scale, outline);
    drawScaledString(invalidatedArea, x, y, str, scale, fill);
}

void GameCanvas::fillRoundedRect(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, int16_t w, int16_t h, int radius, touchgfx::colortype col) const
{
    if (radius < 1)
    {
        touchgfx::Rect rect(x, y, w, h);
        touchgfx::Rect drawRect = rect & invalidatedArea;
        if (drawRect.width > 0 && drawRect.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawRect, col);
        }
        return;
    }

    if (radius * 2 > w) radius = w / 2;
    if (radius * 2 > h) radius = h / 2;

    for (int row = 0; row < h; row++)
    {
        int inset = 0;
        if (row < radius)
        {
            int dy = radius - row - 1;
            inset = radius - (int)sqrtf((float)(radius * radius - dy * dy));
        }
        else if (row >= h - radius)
        {
            int dy = row - (h - radius);
            inset = radius - (int)sqrtf((float)(radius * radius - dy * dy));
        }

        touchgfx::Rect rowRect(x + inset, y + row, w - inset * 2, 1);
        touchgfx::Rect drawRect = rowRect & invalidatedArea;
        if (drawRect.width > 0 && drawRect.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawRect, col);
        }
    }
}

void GameCanvas::drawButton(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, int16_t w, int16_t h, const char* label, bool selected) const
{
    touchgfx::colortype fill = selected ? touchgfx::Color::getColorFromRGB(126, 0, 138) : touchgfx::Color::getColorFromRGB(190, 0, 190);
    touchgfx::colortype border = selected ? touchgfx::Color::getColorFromRGB(255, 245, 255) : touchgfx::Color::getColorFromRGB(80, 80, 90);

    fillRoundedRect(invalidatedArea, x + 4, y + 4, w, h, 8, touchgfx::Color::getColorFromRGB(35, 35, 45));
    fillRoundedRect(invalidatedArea, x, y, w, h, 8, border);
    fillRoundedRect(invalidatedArea, x + 3, y + 3, w - 6, h - 6, 6, fill);

    int textLen = (int)strlen(label);
    int textW = textLen * 16;
    drawScaledString(invalidatedArea, x + (w - textW) / 2, y + (h - 14) / 2, label, 2, touchgfx::Color::getColorFromRGB(255, 255, 255));
}

void GameCanvas::drawStartOverlay(const touchgfx::Rect& invalidatedArea) const
{
    touchgfx::Bitmap startPlanets(BITMAP_PLANETS_3_ID);
    int startPlanetH = startPlanets.getHeight();
    int startPlanetScrollY = (backgroundY / 3) % startPlanetH;
    for (int y = -startPlanetH; y < 320; y += startPlanetH)
    {
        drawBitmap(invalidatedArea, -40, y + startPlanetScrollY, BITMAP_PLANETS_3_ID);
    }

    touchgfx::Rect shade(0, 0, 240, 320);
    touchgfx::Rect drawShade = shade & invalidatedArea;
    if (drawShade.width > 0 && drawShade.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawShade, touchgfx::Color::getColorFromRGB(0, 0, 0), 60);
    }

    touchgfx::colortype white = touchgfx::Color::getColorFromRGB(255, 255, 255);
    touchgfx::colortype black = touchgfx::Color::getColorFromRGB(0, 0, 0);
    drawOutlinedScaledString(invalidatedArea, 8, 58, "SPACE INVADERS", 2, white, black);

    float shipWave = sinf((float)backgroundY * 0.035f);
    int shipX = (240 - playerWidth) / 2 + (int)(shipWave * 42.0f);
    int shipY = 150 + (int)(sinf((float)backgroundY * 0.022f) * 8.0f);
    int pFlameH = 10 + (rand() % 8);
    touchgfx::colortype flameCol = touchgfx::Color::getColorFromRGB(255, 100 + (rand() % 80), 0);
    for (int row = 0; row < pFlameH; row++)
    {
        int w = (pFlameH - row) * 8 / pFlameH;
        if (w > 0)
        {
            touchgfx::Rect rowRect(shipX + playerWidth / 2 - w / 2, shipY + playerHeight - 3 + row, w, 1);
            touchgfx::Rect drawRect = rowRect & invalidatedArea;
            if (drawRect.width > 0 && drawRect.height > 0)
            {
                touchgfx::HAL::lcd().fillRect(drawRect, flameCol);
            }
        }
    }
    drawBitmap(invalidatedArea, shipX, shipY, BITMAP_PLAYER_SHIP_1_ID);

    drawButton(invalidatedArea, 54, 246, 132, 38, "PLAY", true);
}

void GameCanvas::drawGameOverOverlay(const touchgfx::Rect& invalidatedArea) const
{
    touchgfx::Rect shade(0, 0, 240, 320);
    touchgfx::Rect drawShade = shade & invalidatedArea;
    if (drawShade.width > 0 && drawShade.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawShade, touchgfx::Color::getColorFromRGB(0, 0, 0), 160);
    }

    drawScaledString(invalidatedArea, 12, 80, "GAME OVER", 3, touchgfx::Color::getColorFromRGB(255, 255, 255));

    char scoreBuf[24] = "SCORE:";
    char scoreVal[12];
    itoa(score, scoreVal, 10);
    strcat(scoreBuf, scoreVal);
    drawScaledString(invalidatedArea, 36, 112, scoreBuf, 2, touchgfx::Color::getColorFromRGB(255, 255, 255));

    if (showLeaderboard)
    {
        drawScaledString(invalidatedArea, 80, 134, "TOP 5", 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
        for (int i = 0; i < 5; i++)
        {
            char row[24];
            char rank[4];
            char value[12];
            itoa(i + 1, rank, 10);
            itoa(leaderboard[i], value, 10);
            strcpy(row, rank);
            strcat(row, ":");
            strcat(row, value);
            drawScaledString(invalidatedArea, 72, 156 + i * 18, row, 2, touchgfx::Color::getColorFromRGB(220, 220, 220));
        }
    }
    else
    {
        drawScaledString(invalidatedArea, 52, 152, "BEST", 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
        char bestVal[12];
        itoa(highScore, bestVal, 10);
        drawScaledString(invalidatedArea, 126, 152, bestVal, 2, touchgfx::Color::getColorFromRGB(255, 255, 100));
    }

    drawButton(invalidatedArea, 34, 248, 172, 32, "RESTART", true);
    drawButton(invalidatedArea, 34, 288, 172, 26, showLeaderboard ? "EXIT" : "TOP 5", false);
}

void GameCanvas::drawStippleFlash(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, uint16_t bitmapId, touchgfx::colortype col) const
{
    touchgfx::Bitmap bmp(bitmapId);
    int16_t w = bmp.getWidth();
    int16_t h = bmp.getHeight();
    
    const uint32_t* data32 = (const uint32_t*)bmp.getData();
    if (!data32)
    {
        return;
    }
    
    touchgfx::Rect rect(x, y, w, h);
    touchgfx::Rect draw = rect & invalidatedArea;
    if (draw.width > 0 && draw.height > 0)
    {
        for (int py = draw.y; py < draw.y + draw.height; py++)
        {
            int bitmapY = py - y;
            for (int px = draw.x; px < draw.x + draw.width; px++)
            {
                int bitmapX = px - x;
                uint32_t pixel = data32[bitmapY * w + bitmapX];
                uint8_t a = (pixel >> 24) & 0xFF; // Lấy Alpha từ pixel ARGB8888
                if (a > 10) // Chỉ tô màu những pixel không trong suốt
                {
                    // Trích xuất các kênh màu gốc
                    uint8_t r_orig = (pixel >> 16) & 0xFF;
                    uint8_t g_orig = (pixel >> 8) & 0xFF;
                    uint8_t b_orig = pixel & 0xFF;
                    
                    // Trích xuất các kênh màu của màu đè (tint)
                    uint8_t r_tint = touchgfx::Color::getRed(col);
                    uint8_t g_tint = touchgfx::Color::getGreen(col);
                    uint8_t b_tint = touchgfx::Color::getBlue(col);
                    
                    // Hòa trộn màu 50% nguyên bản + 50% màu tint
                    uint8_t r_final = (r_orig + r_tint) / 2;
                    uint8_t g_final = (g_orig + g_tint) / 2;
                    uint8_t b_final = (b_orig + b_tint) / 2;
                    
                    touchgfx::colortype finalCol = touchgfx::Color::getColorFromRGB(r_final, g_final, b_final);
                    
                    // Vẽ pixel đã nhuộm màu với độ trong suốt gốc
                    touchgfx::Rect pixelRect(px, py, 1, 1);
                    touchgfx::HAL::lcd().fillRect(pixelRect, finalCol, a);
                }
            }
        }
    }
}

void GameCanvas::drawBoss(const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, bool isHitFlash) const
{
    touchgfx::Bitmap bmp(BITMAP_NAUTOLAN_SHIP_TORPEDO_SHIP_ID);
    int16_t w = bmp.getWidth(); // 64
    int16_t h = bmp.getHeight(); // 64
    const uint32_t* data32 = (const uint32_t*)bmp.getData();
    if (!data32) return;
    
    // Draw 2 glowing cyan/green engine exhausts at the top/rear of the Boss (flickering, aligned with 128x128 size)
    {
        int16_t exW = 8; // Làm cột lửa thon gọn hơn (rộng 8px)
        int16_t exH = 18 + (gameTicks % 4) * 4; // Lập lòe độ dài
        
        touchgfx::colortype exCol1 = touchgfx::Color::getColorFromRGB(0, 255, 200);
        touchgfx::colortype exCol2 = touchgfx::Color::getColorFromRGB(80, 240, 255);
        touchgfx::colortype finalExCol = ((gameTicks / 2) % 2 == 0) ? exCol1 : exCol2;
        
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

void GameCanvas::draw(const touchgfx::Rect& invalidatedArea) const
{
    // Determine which level assets to draw based on transition state
    int drawLevel = level;
    if (levelTransitionTimer > 60)
    {
        drawLevel = level - 1;
        if (drawLevel < 1) drawLevel = 1;
    }

    uint16_t bgBmpId = BITMAP_BACKGROUND_1_ID;
    uint16_t nebulaBmpId = BITMAP_NEBULA_1_ID;
    uint16_t dustBmpId = BITMAP_DUST_1_ID;
    uint16_t starsBmpId = BITMAP_STARS_1_ID;
    uint16_t planetsBmpId = BITMAP_PLANETS_1_ID;

    if (drawLevel == 2)
    {
        bgBmpId = BITMAP_BACKGROUND_2_ID;
        nebulaBmpId = BITMAP_NEBULA_2_ID;
        dustBmpId = BITMAP_DUST_2_ID;
        starsBmpId = BITMAP_STARS_2_ID;
        planetsBmpId = BITMAP_PLANETS_2_ID;
    }
    else if (drawLevel == 3)
    {
        bgBmpId = BITMAP_BACKGROUND_3_ID;
        nebulaBmpId = BITMAP_NEBULA_3_ID;
        dustBmpId = BITMAP_DUST_3_ID;
        starsBmpId = BITMAP_STARS_3_ID;
        planetsBmpId = BITMAP_PLANETS_3_ID;
    }
    else if (drawLevel == 4)
    {
        bgBmpId = BITMAP_BACKGROUND_4_ID;
        nebulaBmpId = BITMAP_NEBULA_4_ID;
        dustBmpId = BITMAP_DUST_4_ID;
        starsBmpId = BITMAP_STARS_4_ID;
        planetsBmpId = BITMAP_PLANETS_4_ID;
    }
    else if (drawLevel == 5)
    {
        bgBmpId = BITMAP_BACKGROUND_5_ID;
        nebulaBmpId = BITMAP_NEBULA_5_ID;
        dustBmpId = BITMAP_DUST_5_ID;
        starsBmpId = BITMAP_STARS_5_ID;
        planetsBmpId = BITMAP_PLANETS_5_ID;
    }

    // 1. Draw multi-layered scrolling background
    // Base Space
    touchgfx::Bitmap bgSpace(bgBmpId);
    int bgSH = bgSpace.getHeight();
    int scrollSpaceY = (backgroundY / 4) % bgSH;
    for (int x = 0; x < 240; x += bgSpace.getWidth())
    {
        for (int y = -bgSH; y < 320; y += bgSH)
        {
            drawBitmap(invalidatedArea, x, y + scrollSpaceY, bgBmpId);
        }
    }

    // Parallax Nebula
    touchgfx::Bitmap bgNebula(nebulaBmpId);
    int bgNH = bgNebula.getHeight();
    int scrollNebulaY = (backgroundY / 3) % bgNH;
    for (int x = 0; x < 240; x += bgNebula.getWidth())
    {
        for (int y = -bgNH; y < 320; y += bgNH)
        {
            drawBitmap(invalidatedArea, x, y + scrollNebulaY, nebulaBmpId);
        }
    }

    // Parallax Dust
    touchgfx::Bitmap bgDust(dustBmpId);
    int bgDH = bgDust.getHeight();
    int scrollDustY = (backgroundY / 2) % bgDH;
    for (int x = 0; x < 240; x += bgDust.getWidth())
    {
        for (int y = -bgDH; y < 320; y += bgDH)
        {
            drawBitmap(invalidatedArea, x, y + scrollDustY, dustBmpId);
        }
    }

    // Parallax Stars
    touchgfx::Bitmap bgStars(starsBmpId);
    int bgStH = bgStars.getHeight();
    int scrollStarsY = (backgroundY) % bgStH;
    for (int x = 0; x < 240; x += bgStars.getWidth())
    {
        for (int y = -bgStH; y < 320; y += bgStH)
        {
            drawBitmap(invalidatedArea, x, y + scrollStarsY, starsBmpId);
        }
    }

    // Planets
    touchgfx::Bitmap bgPlanets(planetsBmpId);
    int bgPH = bgPlanets.getHeight();
    int scrollPlanetsY = (backgroundY / 6) % (320 + bgPH);
    drawBitmap(invalidatedArea, 10, -bgPH + scrollPlanetsY, planetsBmpId);

    if (screenState == STATE_START)
    {
        drawStartOverlay(invalidatedArea);
        return;
    }

    // Full-screen flash effect for Ultimate Bomb (drawn behind sprites)
    if (flashScreenTimer > 0)
    {
        touchgfx::colortype flashCol = (flashScreenTimer > 4) ? 
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
            touchgfx::Rect rowRect(playerX + playerWidth / 2 - w / 2, playerY + playerHeight - 3 + row, w, 1);
            touchgfx::Rect drawRect = rowRect & invalidatedArea;
            if (drawRect.width > 0 && drawRect.height > 0)
            {
                touchgfx::HAL::lcd().fillRect(drawRect, pFlameCol);
            }
        }
    }

    // 3. Draw barrier shield sprite if shield is active
    if (hasShield)
    {
        int16_t bx = playerX - (48 - playerWidth) / 2;
        int16_t by = playerY - (48 - playerHeight) / 2;
        drawBitmap(invalidatedArea, bx, by, BITMAP_BARRIER_ID);
    }

    // 4. Draw player ship
    drawBitmap(invalidatedArea, playerX, playerY, BITMAP_PLAYER_SHIP_1_ID);
    if (playerHitFlashTimer > 0)
    {
        drawStippleFlash(invalidatedArea, playerX, playerY, BITMAP_PLAYER_SHIP_1_ID, touchgfx::Color::getColorFromRGB(0, 100, 255));
    }

    // 5. Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            if (bullets[i].isBallBullet)
            {
                float pulse = sinf((float)bullets[i].animFrame * 0.25f);
                int r = 4 + (int)(pulse * 1.5f);
                touchgfx::colortype outerCol = touchgfx::Color::getColorFromRGB(255, 170, 0);
                drawCircle(invalidatedArea, (int16_t)bullets[i].x, (int16_t)bullets[i].y, r, outerCol);
                touchgfx::colortype innerCol = touchgfx::Color::getColorFromRGB(255, 255, 150);
                int ir = r / 2;
                if (ir > 0)
                {
                    drawCircle(invalidatedArea, (int16_t)bullets[i].x, (int16_t)bullets[i].y, ir, innerCol);
                }
            }
            else
            {
                drawBitmap(invalidatedArea, (int16_t)bullets[i].x, (int16_t)bullets[i].y, BITMAP_SHOOT_1_ID);
            }
        }
    }

    // 6. Draw enemy engine flames
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            int eFlameH = 6 + (rand() % 6);
            touchgfx::colortype eFlameCol = touchgfx::Color::getColorFromRGB(0, 220 + (rand() % 35), 220);
            bool dualEngines = (enemies[i].type == ENEMY_BALL_SHOOTER || enemies[i].type == ENEMY_DIAGONAL_DASH);
            
            if (dualEngines)
            {
                int lx = (int)enemies[i].x + enemies[i].width / 4;
                for (int row = 0; row < eFlameH; row++)
                {
                    int w = (eFlameH - row) * 4 / eFlameH;
                    if (w > 0)
                    {
                        touchgfx::Rect rowRect(lx - w / 2, (int)enemies[i].y + 14 - row, w, 1);
                        touchgfx::Rect drawRect = rowRect & invalidatedArea;
                        if (drawRect.width > 0 && drawRect.height > 0)
                        {
                            touchgfx::HAL::lcd().fillRect(drawRect, eFlameCol);
                        }
                    }
                }
                int rx = (int)enemies[i].x + (enemies[i].width * 3) / 4;
                for (int row = 0; row < eFlameH; row++)
                {
                    int w = (eFlameH - row) * 4 / eFlameH;
                    if (w > 0)
                    {
                        touchgfx::Rect rowRect(rx - w / 2, (int)enemies[i].y + 14 - row, w, 1);
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
                int cx = (int)enemies[i].x + enemies[i].width / 2;
                for (int row = 0; row < eFlameH; row++)
                {
                    int w = (eFlameH - row) * 5 / eFlameH;
                    if (w > 0)
                    {
                        touchgfx::Rect rowRect(cx - w / 2, (int)enemies[i].y + 14 - row, w, 1);
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
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            uint16_t enemyBmpId = BITMAP_ENEMY_SHIP_1_ID;
            switch (enemies[i].type)
            {
            case ENEMY_TARGETED: enemyBmpId = BITMAP_ENEMY_SHIP_TARGETED_ID; break;
            case ENEMY_ZIGZAG: enemyBmpId = BITMAP_ENEMY_SHIP_ZIGZAG_ID; break;
            case ENEMY_HORIZONTAL: enemyBmpId = BITMAP_ENEMY_SHIP_HORIZONTAL_ID; break;
            case ENEMY_BURST_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_STRAIGHT_ID; break;
            case ENEMY_BALL_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_SPIRAL_ID; break;
            case ENEMY_DIAGONAL_DASH: enemyBmpId = BITMAP_ENEMY_SHIP_SHOOTER_ID; break;
            }
            drawBitmap(invalidatedArea, (int16_t)enemies[i].x, (int16_t)enemies[i].y, enemyBmpId);
            if (enemies[i].hitFlashTimer > 0)
            {
                drawStippleFlash(invalidatedArea, (int16_t)enemies[i].x, (int16_t)enemies[i].y, enemyBmpId, touchgfx::Color::getColorFromRGB(255, 0, 0));
            }
        }
    }

    // 7b. Draw Nautolan Torpedo Boss Ship
    if (bossActive)
    {
        drawBoss(invalidatedArea, (int16_t)bossX, (int16_t)bossY, bossHitFlashTimer > 0);
    }

    // 8. Draw asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            drawBitmap(invalidatedArea, (int16_t)asteroids[i].x, (int16_t)asteroids[i].y, asteroids[i].bitmapId);
        }
    }

    // 9. Draw cropped item bonuses (using direct sprites!)
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (items[i].active)
        {
            uint16_t itemBmpId = BITMAP_ITEM_WEAPON_ID;
            switch (items[i].type)
            {
            case ITEM_HEAL: itemBmpId = BITMAP_ITEM_HEAL_ID; break;
            case ITEM_SHIELD: itemBmpId = BITMAP_ITEM_SHIELD_ID; break;
            case ITEM_ULT: itemBmpId = BITMAP_ITEM_ULT_ID; break;
            case ITEM_WEAPON: itemBmpId = BITMAP_ITEM_WEAPON_ID; break;
            }
            drawBitmap(invalidatedArea, (int16_t)items[i].x - 9, (int16_t)items[i].y - 9, itemBmpId); // 18x18 centered
        }
    }

    // 10. Draw Sprite round explosion animations
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosions[i].active)
        {
            int frameIdx = explosions[i].frame;
            if (frameIdx >= 0 && frameIdx < 10)
            {
                uint16_t expBmpId = BITMAP_EXP_ROUND_0_ID + frameIdx;
                touchgfx::Bitmap expBmp(expBmpId);
                int16_t ex = (int16_t)explosions[i].x - expBmp.getWidth() / 2;
                int16_t ey = (int16_t)explosions[i].y - expBmp.getHeight() / 2;
                drawBitmap(invalidatedArea, ex, ey, expBmpId);
            }
        }
    }

    // Draw Boss Health Bar at the bottom if Boss is Active
    if (bossActive)
    {
        // Draw boss indicator text: "CHEFÃO" (using "CHEF AO" with custom tildes drawn on top)
        drawString(invalidatedArea, 10, 305, "CHEF AO", touchgfx::Color::getColorFromRGB(255, 255, 255));
        
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
        
        // Bright red foreground bar scaling with bossHp / bossMaxHp
        int bossHpW = (bossHp * 155) / bossMaxHp;
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
    drawBitmap(invalidatedArea, 6, 2, BITMAP_PLAYER_SHIP_MINI_ID);
    char livesBuf[8];
    itoa(lives, livesBuf, 10);
    drawString(invalidatedArea, 26, 6, "X", touchgfx::Color::getColorFromRGB(255, 255, 255));
    drawString(invalidatedArea, 34, 6, livesBuf, touchgfx::Color::getColorFromRGB(255, 255, 255));

    // Health Bar underneath lives (Magenta bar)
    touchgfx::Rect hpBg(6, 18, 50, 4);
    touchgfx::Rect drawHpBg = hpBg & invalidatedArea;
    if (drawHpBg.width > 0 && drawHpBg.height > 0)
    {
        touchgfx::HAL::lcd().fillRect(drawHpBg, touchgfx::Color::getColorFromRGB(50, 0, 20));
    }
    int hpW = (health * 50) / 100;
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
    itoa(score, scoreVal, 10);
    strcpy(scoreBuf, "SCORE:");
    strcat(scoreBuf, scoreVal);
    drawString(invalidatedArea, 75, 4, scoreBuf, touchgfx::Color::getColorFromRGB(255, 255, 255));

    // Record / HighScore underneath score
    char recordBuf[32];
    char recordVal[16];
    itoa(highScore, recordVal, 10);
    strcpy(recordBuf, "BEST:");
    strcat(recordBuf, recordVal);
    drawString(invalidatedArea, 75, 16, recordBuf, touchgfx::Color::getColorFromRGB(180, 180, 180));

    // C. Difficulty level, Ultimate and Timer on the Right
    int totalSec = gameTicks / 60;
    int min = totalSec / 60;
    int sec = totalSec % 60;
    int centis = (gameTicks % 60) * 100 / 60;

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

    drawString(invalidatedArea, 178, 4, timeBuf, touchgfx::Color::getColorFromRGB(255, 255, 100));

    // Difficulty Level (LV) based on level variable
    char lvBuf[16] = "LV";
    itoa(level, tmp, 10);
    strcat(lvBuf, tmp);
    drawString(invalidatedArea, 178, 16, lvBuf, touchgfx::Color::getColorFromRGB(100, 255, 255));

    // Draw mini ultimate icons next to LV
    for (int u = 0; u < ultCount; u++)
    {
        drawBitmap(invalidatedArea, 204 + u * 12, 16, BITMAP_ITEM_ULT_MINI_ID);
    }

    // Draw Level Transition text banner (centered)
    if (levelTransitionTimer > 0)
    {
        // Dark translucent overlay box in the center
        touchgfx::Rect box(20, 130, 200, 40);
        touchgfx::Rect drawBox = box & invalidatedArea;
        if (drawBox.width > 0 && drawBox.height > 0)
        {
            touchgfx::HAL::lcd().fillRect(drawBox, touchgfx::Color::getColorFromRGB(10, 10, 25));
        }

        // Draw transition text
        if (levelTransitionTimer > 60)
        {
            drawString(invalidatedArea, 56, 146, "HYPERJUMP...", touchgfx::Color::getColorFromRGB(255, 255, 100));
        }
        else
        {
            char stageText[32] = "STAGE ";
            char num[8];
            itoa(level, num, 10);
            strcat(stageText, num);
            strcat(stageText, " START");
            drawString(invalidatedArea, 68, 146, stageText, touchgfx::Color::getColorFromRGB(100, 255, 100));
        }
    }

    if (screenState == STATE_GAME_OVER)
    {
        drawGameOverOverlay(invalidatedArea);
    }
}

void GameCanvas::handleClickEvent(const touchgfx::ClickEvent& event)
{
    if (event.getType() == touchgfx::ClickEvent::PRESSED)
    {
        if (screenState == STATE_START)
        {
            int x = event.getX();
            int y = event.getY();
            if (x >= 54 && x <= 186 && y >= 246 && y <= 284)
            {
                resetGame();
            }
            invalidate();
            return;
        }

        if (screenState == STATE_GAME_OVER)
        {
            int x = event.getX();
            int y = event.getY();
            if (x >= 34 && x <= 206 && y >= 248 && y <= 280)
            {
                resetGame();
            }
            else if (x >= 34 && x <= 206 && y >= 288 && y <= 314)
            {
                showLeaderboard = !showLeaderboard;
            }
            invalidate();
            return;
        }

        playerX = event.getX() - playerWidth / 2;
        playerY = event.getY() - playerHeight / 2;
        invalidate();
    }
}

void GameCanvas::handleDragEvent(const touchgfx::DragEvent& event)
{
    if (screenState == STATE_START)
    {
        return;
    }

    if (screenState == STATE_GAME_OVER)
    {
        return;
    }

    playerX = event.getNewX() - playerWidth / 2;
    playerY = event.getNewY() - playerHeight / 2;

    // Bounds checking
    if (playerX < 0) playerX = 0;
    if (playerX > 240 - playerWidth) playerX = 240 - playerWidth;
    if (playerY < 32) playerY = 32;
    if (playerY > 320 - playerHeight) playerY = 320 - playerHeight;

    invalidate();
}

void GameCanvas::update()
{
    if (screenState == STATE_START)
    {
        backgroundY += 2;
        invalidate();
        return;
    }

    if (screenState == STATE_GAME_OVER)
    {
        backgroundY += 1;
        invalidate();
        return;
    }

    // Tick game time
    gameTicks++;

    // Trigger transitions based on backgroundY distance milestones
    if (backgroundY >= 1200 && level == 1 && levelTransitionTimer == 0)
    {
        level = 2;
        levelTransitionTimer = 120; // 2 seconds (120 frames at 60fps)
    }
    else if (backgroundY >= 2400 && level == 2 && levelTransitionTimer == 0)
    {
        level = 3;
        levelTransitionTimer = 120;
    }
    else if (backgroundY >= 3600 && level == 3 && levelTransitionTimer == 0)
    {
        level = 4;
        levelTransitionTimer = 120;
    }
    else if (backgroundY >= 4800 && level == 4 && levelTransitionTimer == 0)
    {
        level = 5;
        levelTransitionTimer = 120;
    }
    else if (backgroundY >= 6000 && !isBossFight && level == 5 && levelTransitionTimer == 0)
    {
        isBossFight = true;
        bossActive = true;
        bossHp = 200; // Tăng máu Boss để phù hợp với đạn cấp độ 3 và kích thước lớn
        bossMaxHp = 200;
        // Boss được vẽ phóng to gấp đôi (64x64 gốc x2), nên kích thước biên là 128x128
        bossWidth = 128;
        bossHeight = 128;
        bossX = 120 - bossWidth / 2; // = 56
        bossY = -bossHeight;
        bossVx = 1.5f;
        bossShootTimer = 0;
        bossHitFlashTimer = 0;
        flashScreenTimer = 15; // flash screen green/red
        victoryTimer = 0;
        
        bossTargetPoint = 0;
        bossPauseTimer = 0;
        bossIsPaused = false;
    }

    int scrollSpeed = 1;
    if (isBossFight)
    {
        scrollSpeed = 0; // Freeze background scrolling during boss battle!
    }
    else if (levelTransitionTimer > 0)
    {
        levelTransitionTimer--;
        if (levelTransitionTimer > 60)
        {
            scrollSpeed = 12; // Warp speed!
        }
        else
        {
            scrollSpeed = 1 + (levelTransitionTimer * 11 / 60); // Decelerating back
        }
    }

    backgroundY += scrollSpeed;

    if (flashScreenTimer > 0)
    {
        flashScreenTimer--;
    }
    if (playerHitFlashTimer > 0)
    {
        playerHitFlashTimer--;
    }

    // Auto-fire player bullets based on weapon level
    if (shootCooldownTimer > 0)
    {
        shootCooldownTimer--;
    }
    else
    {
        spawnBullet();
        shootCooldownTimer = 15;
    }

    // Update bullets (both player and enemy)
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;
            if (bullets[i].isBallBullet)
            {
                bullets[i].animFrame++;
            }
            if (bullets[i].y < 32 || bullets[i].y > 320 || bullets[i].x < -20 || bullets[i].x > 260)
            {
                bullets[i].active = false;
            }
        }
    }

    // Update dropped items
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (items[i].active)
        {
            items[i].y += items[i].vy;
            
            // Collision with Player
            if (checkCollision((int)items[i].x - 9, (int)items[i].y - 9, 18, 18,
                               playerX, playerY, playerWidth, playerHeight))
            {
                items[i].active = false;
                
                switch (items[i].type)
                {
                case ITEM_HEAL:
                    health = (health + 50 > 100) ? 100 : health + 50;
                    score += 200;
                    break;
                case ITEM_SHIELD:
                    hasShield = true;
                    score += 200;
                    break;
                case ITEM_ULT:
                    ultCount = (ultCount < 3) ? ultCount + 1 : 3;
                    score += 200;
                    break;
                case ITEM_WEAPON:
                    weaponLevel = (weaponLevel < 3) ? weaponLevel + 1 : 3;
                    score += 500;
                    break;
                }
                
                if (score > highScore) highScore = score;
            }
            else if (items[i].y > 320)
            {
                items[i].active = false;
            }
        }
    }

    // Spawn enemies (suppressed during hyperjump and boss fight)
    if (levelTransitionTimer > 60 || isBossFight)
    {
        // Don't spawn new enemies during warp or boss fight!
    }
    else
    {
        if (enemySpawnTimer > 0)
        {
            enemySpawnTimer--;
        }
        else
        {
            spawnEnemy();
            enemySpawnTimer = 45;
        }
    }

    // Update enemies behavior
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
        {
            enemies[i].time += 1.0f / 60.0f;
            float dx, dy, dist;

            switch (enemies[i].type)
            {
            case ENEMY_TARGETED:
                dx = playerX - enemies[i].x;
                dy = playerY - enemies[i].y;
                dist = sqrtf(dx * dx + dy * dy);
                if (dist > 5.0f)
                {
                    enemies[i].x += (dx / dist) * (20.0f / 60.0f);
                    enemies[i].y += (dy / dist) * (20.0f / 60.0f);
                }
                else
                {
                    enemies[i].y += 20.0f / 60.0f;
                }
                break;

            case ENEMY_ZIGZAG:
                enemies[i].x = enemies[i].spawnX + sinf(enemies[i].time * 3.0f) * 60.0f;
                enemies[i].y += 30.0f / 60.0f;
                break;

            case ENEMY_HORIZONTAL:
                enemies[i].x += enemies[i].vx * (35.0f / 60.0f);
                enemies[i].y = enemies[i].spawnY - sinf(enemies[i].time * 1.5f) * 80.0f;
                
                if ((enemies[i].vx > 0 && enemies[i].x > 240) || (enemies[i].vx < 0 && enemies[i].x < -enemies[i].width) || enemies[i].y > 320)
                {
                    enemies[i].active = false;
                }
                break;

            case ENEMY_BURST_SHOOTER:
                enemies[i].y += 12.0f / 60.0f;
                enemies[i].shootTimer++;
                if (enemies[i].shootTimer >= 180)
                {
                    enemies[i].shootTimer = 0;
                    float centerX = enemies[i].x + enemies[i].width / 2.0f;
                    float baseY = enemies[i].y + enemies[i].height;
                    
                    for (int step = 0; step < 4; step++)
                    {
                        for (int b = 0; b < MAX_BULLETS; b++)
                        {
                            if (!bullets[b].active)
                            {
                                bullets[b].active = true;
                                bullets[b].isEnemyBullet = true;
                                bullets[b].isBallBullet = true;
                                bullets[b].animFrame = step * 3;
                                bullets[b].width = 10;
                                bullets[b].height = 10;
                                
                                if (step == 0)      { bullets[b].x = centerX;       bullets[b].y = baseY; }
                                else if (step == 1) { bullets[b].x = centerX - 4.0f;  bullets[b].y = baseY - 10.0f; }
                                else if (step == 2) { bullets[b].x = centerX + 4.0f;  bullets[b].y = baseY - 20.0f; }
                                else if (step == 3) { bullets[b].x = centerX;       bullets[b].y = baseY - 30.0f; }
                                
                                bullets[b].vx = 0.0f;
                                bullets[b].vy = 1.2f;
                                break;
                            }
                        }
                    }
                }
                break;

            case ENEMY_BALL_SHOOTER:
                enemies[i].y += 15.0f / 60.0f;
                enemies[i].shootTimer++;
                if (enemies[i].shootTimer >= 160)
                {
                    enemies[i].shootTimer = 0;
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].isEnemyBullet = true;
                            bullets[b].isBallBullet = true;
                            bullets[b].animFrame = 0;
                            bullets[b].width = 10;
                            bullets[b].height = 10;
                            bullets[b].x = enemies[i].x + (enemies[i].width - bullets[b].width) / 2;
                            bullets[b].y = enemies[i].y + enemies[i].height;
                            
                            float bx = playerX + playerWidth / 2 - bullets[b].x;
                            float by = playerY + playerHeight / 2 - bullets[b].y;
                            float bdist = sqrtf(bx * bx + by * by);
                            if (bdist > 0)
                            {
                                bullets[b].vx = (bx / bdist) * 0.8f;
                                bullets[b].vy = (by / bdist) * 0.8f;
                            }
                            else
                            {
                                bullets[b].vx = 0.0f;
                                bullets[b].vy = 0.8f;
                            }
                            break;
                        }
                    }
                }
                break;

            case ENEMY_DIAGONAL_DASH:
            default:
                enemies[i].x += enemies[i].vx * (150.0f / 60.0f);
                enemies[i].y += enemies[i].vy * (150.0f / 60.0f);
                break;
            }

            if (enemies[i].hitFlashTimer > 0)
            {
                enemies[i].hitFlashTimer--;
            }

            if (enemies[i].type != ENEMY_HORIZONTAL && enemies[i].y > 320)
            {
                enemies[i].active = false;
            }
        }
    }

    // Spawn asteroids (suppressed during hyperjump and boss fight)
    if (levelTransitionTimer > 60 || isBossFight)
    {
        // Don't spawn new asteroids during warp or boss fight!
    }
    else
    {
        if (asteroidSpawnTimer > 0)
        {
            asteroidSpawnTimer--;
        }
        else
        {
            spawnAsteroid();
            asteroidSpawnTimer = 480;
        }
    }

    // Update asteroids
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroids[i].active)
        {
            asteroids[i].y += asteroids[i].vy;
            if (asteroids[i].y > 320)
            {
                asteroids[i].active = false;
            }
        }
    }

    // Update sprite explosions
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (explosions[i].active)
        {
            explosions[i].frame++;
            if (explosions[i].frame >= 10)
            {
                explosions[i].active = false;
            }
        }
    }

    // Bullet Collisions (Forgiving shrunken hitboxes)
    for (int b = 0; b < MAX_BULLETS; b++)
    {
        if (!bullets[b].active) continue;

        if (bullets[b].isEnemyBullet)
        {
            if (checkCollision((int)bullets[b].x + 2, (int)bullets[b].y + 2, bullets[b].width - 4, bullets[b].height - 4,
                               playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12))
            {
                bullets[b].active = false;
                
                if (hasShield)
                {
                    hasShield = false; // Shield absorbed!
                }
                else
                {
                    playerHitFlashTimer = 15; // Nhấp nháy xanh lá trong 15 khung hình
                    int dmg = isBossFight ? 10 : 20;
                    health -= dmg; // Giảm sát thương của đạn khi đấu Boss
                    if (health <= 0)
                    {
                        lives--;
                        health = 100;
                        weaponLevel = 1; // reset weapon level on death
                        if (lives < 0)
                        {
                            endGame();
                            return;
                        }
                    }
                }
            }
        }
        else
        {
            for (int e = 0; e < MAX_ENEMIES; e++)
            {
                if (!enemies[e].active) continue;
                if (checkCollision((int)bullets[b].x + 2, (int)bullets[b].y + 2, bullets[b].width - 4, bullets[b].height - 4,
                                   (int)enemies[e].x + 6, (int)enemies[e].y + 6, enemies[e].width - 12, enemies[e].height - 12))
                {
                    bullets[b].active = false;
                    enemies[e].hp--;
                    enemies[e].hitFlashTimer = 5; // Nhấp nháy đỏ trong 5 khung hình

                    if (enemies[e].hp <= 0)
                    {
                        enemies[e].active = false;

                        // Trigger Explosion
                        for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
                        {
                            if (!explosions[ex].active)
                            {
                                explosions[ex].active = true;
                                explosions[ex].x = enemies[e].x + enemies[e].width / 2;
                                explosions[ex].y = enemies[e].y + enemies[e].height / 2;
                                explosions[ex].frame = 0;
                                break;
                            }
                        }

                        // Reward Score
                        score += 100;
                        if (score > highScore) highScore = score;

                        // 15% Chance to drop bonus items
                        if (rand() % 100 < 15)
                        {
                            for (int it = 0; it < MAX_ITEMS; it++)
                            {
                                if (!items[it].active)
                                {
                                    items[it].active = true;
                                    items[it].x = enemies[e].x + enemies[e].width / 2;
                                    items[it].y = enemies[e].y + enemies[e].height / 2;
                                    items[it].vy = 1.0f;
                                    
                                    // Choose item type based on random weights
                                    int roll = rand() % 100;
                                    if (roll < 35)      items[it].type = ITEM_WEAPON; // 35% chance
                                    else if (roll < 60) items[it].type = ITEM_HEAL;   // 25% chance
                                    else if (roll < 80) items[it].type = ITEM_SHIELD; // 20% chance
                                    else                items[it].type = ITEM_ULT;    // 20% chance
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    // Player collision with Enemy or Asteroid (Forgiving shrunken hitboxes)
    for (int e = 0; e < MAX_ENEMIES; e++)
    {
        if (enemies[e].active && checkCollision(playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12,
                                                (int)enemies[e].x + 6, (int)enemies[e].y + 6, enemies[e].width - 12, enemies[e].height - 12))
        {
            enemies[e].active = false;
            // Spawn explosion for enemy
            for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
            {
                if (!explosions[ex].active)
                {
                    explosions[ex].active = true;
                    explosions[ex].x = enemies[e].x + enemies[e].width / 2;
                    explosions[ex].y = enemies[e].y + enemies[e].height / 2;
                    explosions[ex].frame = 0;
                    break;
                }
            }
            
            if (hasShield)
            {
                hasShield = false; // Shield absorbed!
            }
            else
            {
                playerHitFlashTimer = 15; // Nhấp nháy xanh lá trong 15 khung hình
                health -= 40; // Heavy collision damage
                if (health <= 0)
                {
                    lives--;
                    health = 100;
                    weaponLevel = 1; // reset weapon level
                    if (lives < 0)
                    {
                        endGame();
                        return;
                    }
                }
            }
        }
    }
    for (int a = 0; a < MAX_ASTEROIDS; a++)
    {
        if (asteroids[a].active && checkCollision(playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12,
                                                  (int)asteroids[a].x + 6, (int)asteroids[a].y + 6, asteroids[a].width - 12, asteroids[a].height - 12))
        {
            asteroids[a].active = false;
            
            if (hasShield)
            {
                hasShield = false; // Shield absorbed!
            }
            else
            {
                playerHitFlashTimer = 15; // Nhấp nháy xanh lá trong 15 khung hình
                health -= 30; // Asteroid damage
                if (health <= 0)
                {
                    lives--;
                    health = 100;
                    weaponLevel = 1; // reset weapon level
                    if (lives < 0)
                    {
                        endGame();
                        return;
                    }
                }
            }
        }
    }
    // --- Boss Fight update loop ---
    if (bossActive)
    {
        if (bossHitFlashTimer > 0)
        {
            bossHitFlashTimer--;
        }

        // 1. Move boss down to start position ty[0] (32) first
        if (bossY < 32.0f && !bossIsPaused && bossTargetPoint == 0)
        {
            bossY += 1.0f;
        }
        else
        {
            // Triangle movement points
            // Triangle movement points (Căn lại cho Boss 128x128 trên màn hình 240x320)
            float tx[3] = {10.0f, 102.0f, 56.0f};
            float ty[3] = {32.0f, 32.0f, 140.0f};
            
            if (bossIsPaused)
            {
                bossPauseTimer--;
                
                // 2. Boss Weapon: Shoot 2 vertical streams of yellow ball bullets
                bossShootTimer++;
                if (bossShootTimer >= 12) // Rapid fire during pause!
                {
                    bossShootTimer = 0;
                    
                    // Left launcher stream (Cánh trái boss 128x128)
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].isEnemyBullet = true;
                            bullets[b].isBallBullet = true;
                            bullets[b].width = 16;
                            bullets[b].height = 16;
                            bullets[b].x = bossX + 20 - 8;
                            bullets[b].y = bossY + 96;
                            bullets[b].vx = 0.0f;
                            bullets[b].vy = 3.5f;
                            bullets[b].animFrame = 0;
                            break;
                        }
                    }
                    
                    // Right launcher stream (Cánh phải boss 128x128 - Cách xa 80 pixel)
                    for (int b = 0; b < MAX_BULLETS; b++)
                    {
                        if (!bullets[b].active)
                        {
                            bullets[b].active = true;
                            bullets[b].isEnemyBullet = true;
                            bullets[b].isBallBullet = true;
                            bullets[b].width = 16;
                            bullets[b].height = 16;
                            bullets[b].x = bossX + 100 - 8;
                            bullets[b].y = bossY + 96;
                            bullets[b].vx = 0.0f;
                            bullets[b].vy = 3.5f;
                            bullets[b].animFrame = 0;
                            break;
                        }
                    }
                }
                
                if (bossPauseTimer <= 0)
                {
                    bossIsPaused = false;
                    bossTargetPoint = (bossTargetPoint + 1) % 3;
                }
            }
            else
            {
                // Move towards current target point
                float dx = tx[bossTargetPoint] - bossX;
                float dy = ty[bossTargetPoint] - bossY;
                float dist = sqrtf(dx * dx + dy * dy);
                
                if (dist < 4.0f)
                {
                    bossIsPaused = true;
                    bossPauseTimer = 180; // 3 seconds pause
                    bossShootTimer = 0;
                }
                else
                {
                    // Move speed = 1.4 pixels per frame
                    bossX += (dx / dist) * 1.4f;
                    bossY += (dy / dist) * 1.4f;
                }
            }
        }

        // 3. Collision: Player Bullets hitting Boss
        for (int b = 0; b < MAX_BULLETS; b++)
        {
            if (bullets[b].active && !bullets[b].isEnemyBullet)
            {
                if (checkCollision((int)bullets[b].x, (int)bullets[b].y, bullets[b].width, bullets[b].height,
                                   (int)bossX + 4, (int)bossY + 4, bossWidth - 8, bossHeight - 8))
                {
                    bullets[b].active = false;
                    bossHp -= 8; // Buff damage cho người chơi để kết liễu nhanh hơn
                    bossHitFlashTimer = 5; // nháy đỏ

                    if (bossHp <= 0)
                    {
                        bossActive = false;
                        
                        // Spawn massive chain of explosions
                        for (int k = 0; k < 6; k++)
                        {
                            for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
                            {
                                if (!explosions[ex].active)
                                {
                                    explosions[ex].active = true;
                                    explosions[ex].x = bossX + (rand() % bossWidth);
                                    explosions[ex].y = bossY + (rand() % bossHeight);
                                    explosions[ex].frame = -k * 3; // staggered delay explosion!
                                    break;
                                }
                            }
                        }

                        score += 10000; // 10k points for boss!
                        if (score > highScore) highScore = score;

                        victoryTimer = 240; // 4 seconds victory screen
                    }
                }
            }
        }

        // 4. Collision: Player Ship colliding with Boss
        if (checkCollision(playerX + 6, playerY + 6, playerWidth - 12, playerHeight - 12,
                           (int)bossX + 4, (int)bossY + 4, bossWidth - 8, bossHeight - 8))
        {
            if (hasShield)
            {
                hasShield = false;
            }
            else
            {
                playerHitFlashTimer = 15;
                health -= 50; // Heavy collision damage
                if (health <= 0)
                {
                    lives--;
                    health = 100;
                    weaponLevel = 1;
                    if (lives < 0)
                    {
                        resetGame();
                        return;
                    }
                }
            }
        }
    }

    // Victory screen countdown timer
    if (victoryTimer > 0)
    {
        victoryTimer--;
        if (victoryTimer == 1)
        {
            resetGame();
            return;
        }
    }

    // Request screen repaint
    invalidate();
}

void GameCanvas::spawnBullet()
{
    touchgfx::Bitmap bulletBmp(BITMAP_SHOOT_1_ID);
    int bW = bulletBmp.getWidth();
    int bH = bulletBmp.getHeight();

    if (weaponLevel == 1)
    {
        // Level 1: Single straight bullet
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].isEnemyBullet = false;
                bullets[i].isBallBullet = false;
                bullets[i].width = bW;
                bullets[i].height = bH;
                bullets[i].x = playerX + (playerWidth - bW) / 2;
                bullets[i].y = playerY - bH;
                bullets[i].vx = 0.0f;
                bullets[i].vy = -6.0f;
                break;
            }
        }
    }
    else if (weaponLevel == 2)
    {
        // Level 2: Double parallel bullets
        int spawned = 0;
        for (int i = 0; i < MAX_BULLETS && spawned < 2; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].isEnemyBullet = false;
                bullets[i].isBallBullet = false;
                bullets[i].width = bW;
                bullets[i].height = bH;
                bullets[i].y = playerY - bH;
                bullets[i].vx = 0.0f;
                bullets[i].vy = -6.0f;
                
                if (spawned == 0)
                    bullets[i].x = playerX + playerWidth / 2 - 8;
                else
                    bullets[i].x = playerX + playerWidth / 2 + 4;
                    
                spawned++;
            }
        }
    }
    else
    {
        // Level 3: Triple spread bullets
        int spawned = 0;
        for (int i = 0; i < MAX_BULLETS && spawned < 3; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].active = true;
                bullets[i].isEnemyBullet = false;
                bullets[i].isBallBullet = false;
                bullets[i].width = bW;
                bullets[i].height = bH;
                bullets[i].x = playerX + (playerWidth - bW) / 2;
                bullets[i].y = playerY - bH;
                
                if (spawned == 0)
                {
                    bullets[i].vx = -1.2f;
                    bullets[i].vy = -5.8f;
                }
                else if (spawned == 1)
                {
                    bullets[i].vx = 0.0f;
                    bullets[i].vy = -6.0f;
                }
                else
                {
                    bullets[i].vx = 1.2f;
                    bullets[i].vy = -5.8f;
                }
                
                spawned++;
            }
        }
    }
}

void GameCanvas::spawnEnemy()
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i].active = true;
            enemies[i].time = 0.0f;
            enemies[i].shootTimer = rand() % 40;
            enemies[i].hitFlashTimer = 0;
            // HP scales with game level: Level 1 -> 1 HP, Level 2 -> 2 HP, Level 3 -> 3 HP
            enemies[i].hp = level;

            int typeRand = rand() % 6;
            if (backgroundY < 1200)
            {
                while (typeRand == ENEMY_BURST_SHOOTER || typeRand == ENEMY_BALL_SHOOTER)
                {
                    typeRand = rand() % 6;
                }
            }
            else if (backgroundY < 2400)
            {
                if ((typeRand == ENEMY_BURST_SHOOTER || typeRand == ENEMY_BALL_SHOOTER) && (rand() % 100 < 70))
                {
                    while (typeRand == ENEMY_BURST_SHOOTER || typeRand == ENEMY_BALL_SHOOTER)
                    {
                        typeRand = rand() % 6;
                    }
                }
            }
            enemies[i].type = (EnemyType)typeRand;
            if (enemies[i].type == ENEMY_BURST_SHOOTER || enemies[i].type == ENEMY_BALL_SHOOTER)
            {
                enemies[i].hp = level + 1;
            }

            uint16_t enemyBmpId = BITMAP_ENEMY_SHIP_1_ID;
            switch (enemies[i].type)
            {
            case ENEMY_TARGETED: enemyBmpId = BITMAP_ENEMY_SHIP_TARGETED_ID; break;
            case ENEMY_ZIGZAG: enemyBmpId = BITMAP_ENEMY_SHIP_ZIGZAG_ID; break;
            case ENEMY_HORIZONTAL: enemyBmpId = BITMAP_ENEMY_SHIP_HORIZONTAL_ID; break;
            case ENEMY_BURST_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_STRAIGHT_ID; break;
            case ENEMY_BALL_SHOOTER: enemyBmpId = BITMAP_ENEMY_SHIP_SPIRAL_ID; break;
            case ENEMY_DIAGONAL_DASH: enemyBmpId = BITMAP_ENEMY_SHIP_SHOOTER_ID; break;
            }

            touchgfx::Bitmap enemyBmp(enemyBmpId);
            int eW = enemyBmp.getWidth();
            int eH = enemyBmp.getHeight();
            enemies[i].width = eW;
            enemies[i].height = eH;

            switch (enemies[i].type)
            {
            case ENEMY_TARGETED:
                enemies[i].x = rand() % (240 - eW);
                enemies[i].y = -eH;
                enemies[i].vy = 1.2f;
                break;
            case ENEMY_ZIGZAG:
                enemies[i].spawnX = 50 + rand() % (140 - eW);
                enemies[i].x = enemies[i].spawnX;
                enemies[i].y = -eH;
                enemies[i].vy = 1.5f;
                break;
            case ENEMY_HORIZONTAL:
                if (rand() % 2 == 0)
                {
                    enemies[i].x = -eW;
                    enemies[i].vx = 1.5f;
                }
                else
                {
                    enemies[i].x = 240;
                    enemies[i].vx = -1.5f;
                }
                enemies[i].y = 120.0f;
                enemies[i].spawnY = enemies[i].y;
                break;
            case ENEMY_BURST_SHOOTER:
                enemies[i].x = rand() % (240 - eW);
                enemies[i].y = -eH;
                enemies[i].vy = 0.8f;
                break;
            case ENEMY_BALL_SHOOTER:
                enemies[i].x = rand() % (240 - eW);
                enemies[i].y = -eH;
                enemies[i].vy = 0.6f;
                break;
            case ENEMY_DIAGONAL_DASH:
            default:
                if (rand() % 2 == 0)
                {
                    enemies[i].x = -eW;
                    enemies[i].vx = 0.707f;
                }
                else
                {
                    enemies[i].x = 240;
                    enemies[i].vx = -0.707f;
                }
                enemies[i].y = -eH;
                enemies[i].vy = 0.707f;
                break;
            }
            break;
        }
    }
}

void GameCanvas::spawnAsteroid()
{
    uint16_t asteroidBmps[] = {
        BITMAP_ASTEROID_BLUE_ID,
        BITMAP_ASTEROID_GREEN_ID,
        BITMAP_ASTEROID_PURPLE_ID,
        BITMAP_ASTEROID_RED_ID,
        BITMAP_ASTEROID_WHITE_ID
    };
    uint16_t selectedBmp = asteroidBmps[rand() % 5];
    touchgfx::Bitmap astBmp(selectedBmp);
    int aW = astBmp.getWidth();
    int aH = astBmp.getHeight();

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (!asteroids[i].active)
        {
            asteroids[i].active = true;
            asteroids[i].bitmapId = selectedBmp;
            asteroids[i].width = aW;
            asteroids[i].height = aH;
            asteroids[i].x = rand() % (240 - aW);
            asteroids[i].y = -aH;
            asteroids[i].vy = 0.5f + (float)(rand() % 10) / 10.0f;
            break;
        }
    }
}

bool GameCanvas::checkCollision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 &&
            x1 + w1 > x2 &&
            y1 < y2 + h2 &&
            y1 + h1 > y2);
}

void GameCanvas::recordLeaderboardScore()
{
    for (int i = 0; i < 5; i++)
    {
        if (score > leaderboard[i])
        {
            for (int j = 4; j > i; j--)
            {
                leaderboard[j] = leaderboard[j - 1];
            }
            leaderboard[i] = score;
            break;
        }
    }
}

void GameCanvas::endGame()
{
    if (score > highScore)
    {
        highScore = score;
    }
    recordLeaderboardScore();
    screenState = STATE_GAME_OVER;
    showLeaderboard = false;
}

void GameCanvas::handleKeyEvent(uint8_t key)
{
    if (screenState == STATE_START)
    {
        if (key == '\r' || key == '\n' || key == ' ')
        {
            resetGame();
        }
        invalidate();
        return;
    }

    if (screenState == STATE_GAME_OVER)
    {
        if (key == '\r' || key == '\n' || key == ' ')
        {
            resetGame();
        }
        else if (key == 'l' || key == 'L' || key == 't' || key == 'T')
        {
            showLeaderboard = !showLeaderboard;
        }
        invalidate();
        return;
    }

    int step = 15;
    if (key == 'w' || key == 'W')
    {
        playerY -= step;
    }
    else if (key == 's' || key == 'S')
    {
        playerY += step;
    }
    else if (key == 'a' || key == 'A')
    {
        playerX -= step;
    }
    else if (key == 'd' || key == 'D')
    {
        playerX += step;
    }
    else if (key == ' ' || key == 32)
    {
        // Ultimate Bomb: Clear all screen enemies and bullets!
        if (ultCount > 0)
        {
            ultCount--;
            flashScreenTimer = 8; // Kích hoạt 8 khung hình nhấp nháy toàn màn hình

            // Kích nổ hàng loạt vụ nổ ngẫu nhiên trên toàn màn hình
            for (int j = 0; j < 15; j++)
            {
                for (int ex = 0; ex < MAX_EXPLOSIONS; ex++)
                {
                    if (!explosions[ex].active)
                    {
                        explosions[ex].active = true;
                        explosions[ex].x = 10 + (rand() % 220);
                        explosions[ex].y = 40 + (rand() % 260);
                        explosions[ex].frame = rand() % 4; // Bắt đầu ở các frame ngẫu nhiên để tạo sự tự nhiên
                        break;
                    }
                }
            }

            for (int e = 0; e < MAX_ENEMIES; e++)
            {
                if (enemies[e].active)
                {
                    enemies[e].active = false;
                    score += 100;
                    if (score > highScore) highScore = score;
                }
            }
            // Clear enemy bullets
            for (int b = 0; b < MAX_BULLETS; b++)
            {
                if (bullets[b].active && bullets[b].isEnemyBullet)
                {
                    bullets[b].active = false;
                }
            }
        }
    }

    // Bounds checking (accounting for top HUD bar)
    if (playerX < 0) playerX = 0;
    if (playerX > 240 - playerWidth) playerX = 240 - playerWidth;
    if (playerY < 32) playerY = 32;
    if (playerY > 320 - playerHeight) playerY = 320 - playerHeight;

    invalidate();
}
