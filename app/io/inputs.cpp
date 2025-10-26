/**
 * @file inputs.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-26
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "inputs.h"

#include "app/io/igpio.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME     inpt
#define LOG_MODULE_LEVEL    (3)
#include <debug/log_libs.h>
//<<----------------------
inputs_t inputs_read()
{
    inputs_t i = {};

    i.f.brake = io_gpio_brake_sens();
    i.f.tacho = io_gpio_tacho();
    i.f.sw1 = io_gpio_sw1();
    i.f.sw2 = io_gpio_sw2();
    i.f.sw3 = io_gpio_sw3();
    i.f.sw4 = io_gpio_sw4();

    return i;
}

void inputs_print(inputs_t i)
{
    LOG_INFO("i.f.brake: %d", i.f.brake);
    LOG_INFO("i.f.tacho: %d", i.f.tacho);
    LOG_INFO("i.f.sw1  : %d", i.f.sw1);
    LOG_INFO("i.f.sw2  : %d", i.f.sw2);
    LOG_INFO("i.f.sw3  : %d", i.f.sw3);
    LOG_INFO("i.f.sw4  : %d", i.f.sw4);
}
