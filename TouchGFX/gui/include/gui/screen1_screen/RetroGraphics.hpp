#ifndef RETROGRAPHICS_HPP
#define RETROGRAPHICS_HPP

#include <touchgfx/hal/Types.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <images/BitmapDatabase.hpp>
#include <touchgfx/Bitmap.hpp>
#include <touchgfx/Color.hpp>
#include <math.h>
#include <cstring>

namespace RetroGraphics {

static inline int getFontIndex(char c) {
    if (c >= 'a' && c <= 'z') c -= 32; // Convert to uppercase
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c == ':') return 36;
    if (c == '-') return 37;
    if (c == '.') return 38;
    if (c == ' ') return 39;
    return 39; // Default to space for unknown chars
}

static const uint8_t retroFont7x7[40][7] = {
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
    {0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63}, // A (10)
    {0x7E, 0x63, 0x63, 0x7E, 0x63, 0x63, 0x7E}, // B (11)
    {0x3E, 0x63, 0x60, 0x60, 0x60, 0x63, 0x3E}, // C (12)
    {0x7C, 0x66, 0x63, 0x63, 0x63, 0x66, 0x7C}, // D (13)
    {0x7F, 0x60, 0x60, 0x7E, 0x60, 0x60, 0x7F}, // E (14)
    {0x7F, 0x60, 0x60, 0x7E, 0x60, 0x60, 0x60}, // F (15)
    {0x3E, 0x63, 0x60, 0x6F, 0x63, 0x63, 0x3E}, // G (16)
    {0x63, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x63}, // H (17)
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E}, // I (18)
    {0x1F, 0x06, 0x06, 0x06, 0x66, 0x66, 0x3C}, // J (19)
    {0x63, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x63}, // K (20)
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7F}, // L (21)
    {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63}, // M (22)
    {0x63, 0x73, 0x7B, 0x6F, 0x67, 0x63, 0x63}, // N (23)
    {0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E}, // O (24)
    {0x7E, 0x63, 0x63, 0x7E, 0x60, 0x60, 0x60}, // P (25)
    {0x3E, 0x63, 0x63, 0x63, 0x6B, 0x66, 0x3F}, // Q (26)
    {0x7E, 0x63, 0x63, 0x7E, 0x6C, 0x66, 0x63}, // R (27)
    {0x3F, 0x60, 0x60, 0x3E, 0x03, 0x03, 0x7E}, // S (28)
    {0x7F, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // T (29)
    {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E}, // U (30)
    {0x63, 0x63, 0x63, 0x36, 0x36, 0x1C, 0x08}, // V (31)
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63}, // W (32)
    {0x63, 0x36, 0x1C, 0x08, 0x1C, 0x36, 0x63}, // X (33)
    {0x63, 0x63, 0x36, 0x1C, 0x18, 0x18, 0x18}, // Y (34)
    {0x7F, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7F}, // Z (35)
    {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00}, // : (36)
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00}, // - (37)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18}, // . (38)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // space (39)
};

static inline void drawBitmap(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, uint16_t bitmapId) {
    touchgfx::Bitmap bmp(bitmapId);
    touchgfx::Rect bmpRect(x, y, bmp.getWidth(), bmp.getHeight());
    touchgfx::Rect dirty = bmpRect & invalidatedArea;
    if (dirty.width > 0 && dirty.height > 0)
    {
        int16_t absoluteX = absoluteRect.x + x;
        int16_t absoluteY = absoluteRect.y + y;
        
        touchgfx::Rect bitmapArea = dirty;
        bitmapArea.x -= x;
        bitmapArea.y -= y;
        
        touchgfx::HAL::lcd().drawPartialBitmap(bmp, absoluteX, absoluteY, bitmapArea, 255);
    }
}

static inline void drawCircle(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t cx, int16_t cy, int r, touchgfx::colortype col) {
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

static inline void drawChar(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, char c, touchgfx::colortype col) {
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

static inline void drawString(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, touchgfx::colortype col) {
    int16_t cx = x;
    while (*str)
    {
        drawChar(absoluteRect, invalidatedArea, cx, y, *str, col);
        cx += 8; // 7 pixels width + 1 pixel spacing
        str++;
    }
}

static inline void drawScaledChar(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, char c, int scale, touchgfx::colortype col) {
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

static inline void drawScaledString(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, int scale, touchgfx::colortype col) {
    int16_t cx = x;
    while (*str)
    {
        drawScaledChar(absoluteRect, invalidatedArea, cx, y, *str, scale, col);
        cx += 8 * scale;
        str++;
    }
}

static inline void drawOutlinedScaledString(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, const char* str, int scale, touchgfx::colortype fill, touchgfx::colortype outline) {
    drawScaledString(absoluteRect, invalidatedArea, x - scale, y, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x + scale, y, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x, y - scale, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x, y + scale, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x - scale, y - scale, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x + scale, y - scale, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x - scale, y + scale, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x + scale, y + scale, str, scale, outline);
    drawScaledString(absoluteRect, invalidatedArea, x, y, str, scale, fill);
}

static inline void fillRoundedRect(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, int16_t w, int16_t h, int radius, touchgfx::colortype col) {
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

static inline void drawButton(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, int16_t w, int16_t h, const char* label, bool selected) {
    touchgfx::colortype fill = selected ? touchgfx::Color::getColorFromRGB(126, 0, 138) : touchgfx::Color::getColorFromRGB(190, 0, 190);
    touchgfx::colortype border = selected ? touchgfx::Color::getColorFromRGB(255, 245, 255) : touchgfx::Color::getColorFromRGB(80, 80, 90);

    fillRoundedRect(absoluteRect, invalidatedArea, x + 4, y + 4, w, h, 8, touchgfx::Color::getColorFromRGB(35, 35, 45));
    fillRoundedRect(absoluteRect, invalidatedArea, x, y, w, h, 8, border);
    fillRoundedRect(absoluteRect, invalidatedArea, x + 3, y + 3, w - 6, h - 6, 6, fill);

    int textLen = (int)strlen(label);
    int textW = textLen * 16;
    drawScaledString(absoluteRect, invalidatedArea, x + (w - textW) / 2, y + (h - 14) / 2, label, 2, touchgfx::Color::getColorFromRGB(255, 255, 255));
}

static inline void drawStippleFlash(const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, uint16_t bitmapId, touchgfx::colortype col) {
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

} // namespace RetroGraphics

#endif // RETROGRAPHICS_HPP
