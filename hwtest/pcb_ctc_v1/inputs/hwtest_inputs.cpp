/**
 * @file hwtest_inputs.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-26
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <sys/time.h>

#include "app/application.h"
#include "app/io/igpio.h"
#include "app/io/inputs.h"
#include "app/ui/controls/Button.hpp"
#include "app/utils/idelay.h"
#include "app/version.h"

#include "bsp/ctc_v1/ui/ShiftRegInd.hpp"

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

static void solenoid_pull(uint16_t pulse_ms)
{
    io_gpio_solenoid(true);
    shftregl_write(1);
    delay_ms(pulse_ms);
    io_gpio_solenoid(false);
    shftregl_write(0);
}

static void play()
{
    LOG_INFO("play");
    solenoid_pull(400);
    delay_ms(1000);
}

static void stop()
{
    LOG_INFO("stop");
    solenoid_pull(200);
    delay_ms(1000);
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

    ShiftRegInd ind = {};
    ind.init();
    ind.set_bit((uint8_t)0, true);
    inputs_t i = inputs_read();
    LOG_INFO("inputs: 0x%02x", i.d8);
    inputs_print(i);

    while (1)
    {
        if (m_btns[0].poll_short_press())
        {
            play();
        }
        if (m_btns[1].poll_short_press())
        {
            stop();
        }

        i = inputs_read();
        ind.set_inputs(i);
        delay_ms(10);
    }
}