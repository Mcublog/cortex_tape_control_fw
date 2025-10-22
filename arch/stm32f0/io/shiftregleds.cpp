/**
 * @file shiftregleds.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-09-20
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "shiftregleds.h"

#include "app/utils/idelay.h"

#include "configs/shiftreg_config.h"

static void latch_set(bool state)
{
    HAL_GPIO_WritePin(OUTLATCH_PORT, OUTLATCH_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void reset_set(bool state)
{
    // HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void oe_set(bool state)
{
    // HAL_GPIO_WritePin(OE_PORT, OE_PIN, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief
 *
 */
void shftregl_init(void)
{
    latch_set(true);
}

/**
 * @brief
 *
 * @param data
 */
void shftregl_write(uint8_t data)
{
    // uint8_t dummy = 0;
    // HAL_SPI_Transmit(&SHIFTREG_SPI_HANDLE, &dummy, 1, HAL_MAX_DELAY);

    HAL_SPI_Transmit(&SHIFTREG_SPI_HANDLE, &data, 1, HAL_MAX_DELAY);
    while (HAL_SPI_GetState(&SHIFTREG_SPI_HANDLE) != HAL_SPI_STATE_READY)
        ;
    // delay_ms(1);
    latch_set(false);
    latch_set(true);
}
