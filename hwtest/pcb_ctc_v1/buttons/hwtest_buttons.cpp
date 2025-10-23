/**
 * @file hwtest_buttons.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-23
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <sys/time.h>

#include "app/application.h"
#include "app/io/igpio.h"
#include "app/ui/controls/Button.hpp"
#include "app/utils/idelay.h"
#include "app/utils/itimer_measurement.h"
#include "app/version.h"
#include "libs/SoftwareTimer/SoftTimers.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME app
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

//>>---------------------- Locals
static constexpr uint8_t kBtnNumber = 2;
static Button m_btns[kBtnNumber] = {Button(egde_t::EDGE_RAISING, 500, io_gpio_btn0),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn1)};
//<<----------------------

static void buttons_process(uint8_t id)
{
    for (uint8_t i = 0; i < kBtnNumber; ++i)
        m_btns[i].process();
    Timer_Start(id, 5);
}

static void buttons_init()
{
    for (uint8_t i = 0; i < kBtnNumber; ++i)
        m_btns[i].init();
}

static void button_timer_init()
{
    uint8_t id = Timer_Create((timer_handler_t)buttons_process);
    Timer_Start(id, 10);
}

/**
 * @brief
 *
 */
void application(void)
{
    LOG_INFO("Version: %s", FW_VERSION);

    buttons_init();
    button_timer_init();

    while (1)
    {
        for (uint8_t i = 0; i < kBtnNumber; ++i)
        {
            if (m_btns[i].poll_long_press())
            {
                LOG_INFO("btn[%d]: long press", i);
                // io_gpio_red_led(i == 0);
            }
            else if (m_btns[i].poll_short_press())
            {
                LOG_INFO("btn[%d]: short press", i);
                // io_gpio_green_led(i == 0);
            }
        }

        delay_ms(100);
    }
}