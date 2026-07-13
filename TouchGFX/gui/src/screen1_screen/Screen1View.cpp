#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    gameCanvas.setPosition(0, 0, 240, 320);
    add(gameCanvas);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{
    Screen1ViewBase::handleTickEvent();
    gameCanvas.update();
}

void Screen1View::handleKeyEvent(uint8_t key)
{
    gameCanvas.handleKeyEvent(key);
}
