/**
 * @file every_10ms.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-12-06
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "app/events/list.hpp"

#include "app/io/igpio.h"
#include "app/ui/controls/Button.hpp"
//>>---------------------- Log control
#define LOG_MODULE_NAME e10ms
#define LOG_MODULE_LEVEL (3)
#include "log_libs.h"
//<<----------------------
//>>---------------------- Locals

static constexpr uint8_t kBtnNumber = 2;
static Button m_btns[kBtnNumber] = {Button(egde_t::EDGE_RAISING, 500, io_gpio_btn0),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn1)};
//<<----------------------

//>>---------------------- Locals
static void button_process()
{
    for (uint8_t i = 0; i < kBtnNumber; ++i)
        m_btns[i].process();
}

static void buttons_init()
{
    for (uint8_t i = 0; i < kBtnNumber; ++i)
        m_btns[i].init();
}
//<<----------------------

//>>---------------------- Public
void Events::init()
{
    buttons_init();
}

void Events::every_10ms()
{
    if (m_btns[0].poll_short_press())
    {
        LOG_INFO("play");
    }
    if (m_btns[1].poll_short_press())
    {
        LOG_INFO("stop");
    }

    button_process();
}
//<<----------------------