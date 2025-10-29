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

bool io_gpio_brake_sens(void)
{
    return HAL_GPIO_ReadPin(BRAKE_SENS_GPIO_Port, BRAKE_SENS_Pin) == GPIO_PIN_SET;
}

bool io_gpio_sw1(void)
{
    return HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == GPIO_PIN_RESET;
}

bool io_gpio_sw2(void)
{
    return HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == GPIO_PIN_RESET;
}

bool io_gpio_sw3(void)
{
    return HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin) == GPIO_PIN_RESET;
}

bool io_gpio_sw4(void)
{
    return HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin) == GPIO_PIN_RESET;
}

bool io_gpio_tacho(void)
{
    return HAL_GPIO_ReadPin(TACHO_GPIO_Port, TACHO_Pin) == GPIO_PIN_RESET;
}
//<<----------------------