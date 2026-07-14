#ifndef GAMECANVAS_HPP
#define GAMECANVAS_HPP

#include <touchgfx/widgets/Widget.hpp>
#include <touchgfx/Color.hpp>

#include <images/BitmapDatabase.hpp>
#include "GameTypes.hpp"
#include "GameLogic.hpp"
#include "GameRenderer.hpp"
class GameCanvas : public touchgfx::Widget
{
public:
    GameCanvas();
    virtual ~GameCanvas() {}

    virtual void draw(const touchgfx::Rect& invalidatedArea) const override;
    virtual touchgfx::Rect getSolidRect() const override;

    // Input handlers
    virtual void handleClickEvent(const touchgfx::ClickEvent& event) override;
    virtual void handleDragEvent(const touchgfx::DragEvent& event) override;
    void handleKeyEvent(uint8_t key);

    // Game loop tick update
    void update();

private:
    // Game state structures are now in GameTypes.hpp
    GameLogic logic;
    GameRenderer renderer;

    // Methods moved to GameRenderer

};

#endif // GAMECANVAS_HPP
