#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#ifndef SIMULATOR
#include "stm32f4xx_hal.h"
#include "main.h"
#endif

Model::Model() : modelListener(0), lastBombState(true)
{

}

void Model::tick()
{
#ifndef SIMULATOR
    if (modelListener)
    {
        // Handle Movement (Trigger every tick for smooth 60fps movement)
        if (HAL_GPIO_ReadPin(BTN_A_GPIO_Port, BTN_A_Pin) == GPIO_PIN_RESET)
        {
            modelListener->hardwareKeyEvent('a');
        }
        if (HAL_GPIO_ReadPin(BTN_D_GPIO_Port, BTN_D_Pin) == GPIO_PIN_RESET)
        {
            modelListener->hardwareKeyEvent('d');
        }
        if (HAL_GPIO_ReadPin(BTN_W_GPIO_Port, BTN_W_Pin) == GPIO_PIN_RESET)
        {
            modelListener->hardwareKeyEvent('w');
        }
        if (HAL_GPIO_ReadPin(BTN_S_GPIO_Port, BTN_S_Pin) == GPIO_PIN_RESET)
        {
            modelListener->hardwareKeyEvent('s');
        }

        // Handle Bomb (PA10) with edge detection
        bool currentBombState = (HAL_GPIO_ReadPin(BTN_SPACE_GPIO_Port, BTN_SPACE_Pin) == GPIO_PIN_RESET);
        if (currentBombState && !lastBombState)
        {
            modelListener->hardwareKeyEvent(' ');
        }
        lastBombState = currentBombState;
    }
#endif
}
