/**
 * @file application.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-23
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "application.h"

#include "app/events/list.hpp"
#include "app/io/igpio.h"
#include "app/process/communication/process.hpp"
#include "app/system/system.h"
#include "app/utils/idelay.h"
#include "app/version.h"

#include "libs/SoftwareTimer/SoftTimers.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME app
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

//>>---------------------- Locals
volatile bool m_flag_1ms    = false;
volatile bool m_flag_10ms   = false;
volatile bool m_flag_100ms  = false;
//<<----------------------
static void flag_every_1ms(uint8_t id)
{
    m_flag_1ms = true;
    Timer_Start(id, 1);
}

static void flag_every_10ms(uint8_t id)
{
    m_flag_10ms = true;
    Timer_Start(id, 10);
}

static void flag_every_100ms(uint8_t id)
{
    m_flag_100ms = true;
    Timer_Start(id, 100);
}

static void start_events_timers()
{
    uint8_t id = Timer_Create((timer_handler_t)flag_every_1ms);
    Timer_Start(id, 1);

    id = Timer_Create((timer_handler_t)flag_every_10ms);
    Timer_Start(id, 10);

    id = Timer_Create((timer_handler_t)flag_every_100ms);
    Timer_Start(id, 100);
}

static void solenoid_pull(uint16_t pulse_ms)
{
    io_gpio_solenoid(true);
    isystem()->indication()->set_bit(0, 1);
    delay_ms(pulse_ms);
    io_gpio_solenoid(false);
    isystem()->indication()->set_bit(0, 0);
}

static void on_solenoid_pull(void *data, uint32_t size)
{
    const sollenoid_pull_t cmd = *((const sollenoid_pull_t *)data);

    LOG_INFO("pull param: pull ms: %d cooldown ms: %d", cmd.pull_time_ms,
             cmd.cooldown_ms);

    solenoid_pull(cmd.pull_time_ms);
    delay_ms(cmd.cooldown_ms);
}

/**
 * @brief
 *
 */
void application(void)
{
    LOG_INFO("Version: %s", FW_VERSION);

    System *sys = isystem();
    if (sys == nullptr)
    {
        LOG_ERROR("sys is not ready");
        System::infitite_loop();
    }
    sys->what();
    sys->init();

    isystem()->indication()->init();
    Events::init();
    start_events_timers();

    Communication::callback_attach_solenoid_pull((command_callback_t)on_solenoid_pull);

    while (1)
    {
        if (m_flag_1ms)
        {
            m_flag_1ms = false;
            bool tacho = io_gpio_tacho();
            isystem()->indication()->set_bit(1, tacho);
            Communication::process();
        }

        if (m_flag_10ms)
        {
            m_flag_10ms = false;
            Events::every_10ms();
        }

        if (m_flag_100ms)
        {
            m_flag_100ms = false;
        }
    }
}