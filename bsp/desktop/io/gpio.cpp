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

}

bool io_gpio_btn0(void)
{
    return true;
}

bool io_gpio_btn1(void)
{
    return true;
}

bool io_gpio_brake_sens(void)
{
    return true;
}

bool io_gpio_sw1(void)
{
    return true;
}

bool io_gpio_sw2(void)
{
    return true;
}

bool io_gpio_sw3(void)
{
    return true;
}

bool io_gpio_sw4(void)
{
    return true;
}

bool io_gpio_tacho(void)
{
    return true;
}

void io_gpio_power(bool on)
{

}
//<<----------------------