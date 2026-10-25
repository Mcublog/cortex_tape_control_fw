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
#include "app/mech/mech.hpp"
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
volatile bool m_flag_2ms    = false;
volatile bool m_flag_10ms   = false;
volatile bool m_flag_20ms   = false;
volatile bool m_flag_100ms  = false;
//<<----------------------
static void flag_every_1ms(uint8_t id)
{
    m_flag_1ms = true;
    Timer_Start(id, 1);
}

static void flag_every_2ms(uint8_t id)
{
    m_flag_2ms = true;
    Timer_Start(id, 2);
}

static void flag_every_10ms(uint8_t id)
{
    m_flag_10ms = true;
    Timer_Start(id, 10);
}

static void flag_every_20ms(uint8_t id)
{
    m_flag_20ms = true;
    Timer_Start(id, 20);
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

    id = Timer_Create((timer_handler_t)flag_every_2ms);
    Timer_Start(id, 2);

    id = Timer_Create((timer_handler_t)flag_every_10ms);
    Timer_Start(id, 10);

    id = Timer_Create((timer_handler_t)flag_every_20ms);
    Timer_Start(id, 20);

    id = Timer_Create((timer_handler_t)flag_every_100ms);
    Timer_Start(id, 100);
}

static bool on_mech_mode(uint32_t mode)
{
    if (!Mech::ready() || (mode > 6))
        return false;

    return Mech::instance().request(static_cast<mech::UserMode>(mode));
}

static void on_mech_state(mech_state_t *state)
{
    if (!Mech::ready() || (state == nullptr))
        return;

    const Crp42602y &mech = Mech::instance();

    state->mode = static_cast<uint32_t>(mech.mode());
    state->target_mode = static_cast<uint32_t>(mech.target_mode());
    state->user_mode = static_cast<uint32_t>(mech.user_mode());
    state->transition = mech.transition_left();
    state->retries = mech.retries();
    state->error = mech.error();
    state->sensors = mech.sensors().to_byte();
    state->last_dir = static_cast<uint32_t>(mech.last_play_dir());
    state->capstan = mech.capstan_on() ? 1 : 0;
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

    Mech::init(*sys->mech_io());

    Communication::callback_attach_mech_mode(on_mech_mode);
    Communication::callback_attach_mech_state(on_mech_state);

    start_events_timers();

    while (1)
    {
        if (m_flag_20ms)
        {
            m_flag_20ms = false;
            Mech::instance().tick20ms();
        }

        if (m_flag_2ms)
        {
            m_flag_2ms = false;
            Mech::instance().tick2ms();
        }

        if (m_flag_10ms)
        {
            m_flag_10ms = false;
            Events::every_10ms();
        }

        if (m_flag_1ms)
        {
            m_flag_1ms = false;
            bool tacho = io_gpio_tacho();
            isystem()->indication()->set_bit(1, tacho);
            Communication::process();
        }

        if (m_flag_100ms)
        {
            m_flag_100ms = false;
        }
    }
}
