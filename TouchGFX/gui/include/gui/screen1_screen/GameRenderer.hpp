#ifndef GAMERENDERER_HPP
#define GAMERENDERER_HPP

#include "GameLogic.hpp"
#include <touchgfx/hal/Types.hpp>

class GameRenderer
{
public:
    void render(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea) const;

private:
    void drawBoss(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea, int16_t x, int16_t y, bool isHitFlash) const;
    void drawStartOverlay(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea) const;
    void drawGameOverOverlay(const GameLogic& logic, const touchgfx::Rect& absoluteRect, const touchgfx::Rect& invalidatedArea) const;
};

#endif // GAMERENDERER_HPP
