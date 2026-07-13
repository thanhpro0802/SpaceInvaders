#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/screen1_screen/GameCanvas.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen() override;
    virtual void tearDownScreen() override;
    virtual void handleTickEvent() override;
    virtual void handleKeyEvent(uint8_t key) override;
protected:
    GameCanvas gameCanvas;
};

#endif // SCREEN1VIEW_HPP
