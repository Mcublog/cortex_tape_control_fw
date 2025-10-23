/**
 * @file gpio.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-22
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "app/io/igpio.h"

#include "main.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME gpio
#if defined(NDEBUG)
#define LOG_MODULE_LEVEL (0)
#else
#define LOG_MODULE_LEVEL (2)
#endif
#include "libs/debug/log_libs.h"
//<<----------------------

/**
 * @brief
 *
 * @param on
 */
void io_gpio_solenoid(bool on)
{
    HAL_GPIO_WritePin(SOLENOID_GPIO_Port, SOLENOID_Pin,
                      on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool io_gpio_btn0(void)
{
    return HAL_GPIO_ReadPin(BTN0_GPIO_Port, BTN0_Pin) == GPIO_PIN_RESET;
}

bool io_gpio_btn1(void)
{
    return HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == GPIO_PIN_RESET;
}
//<<----------------------