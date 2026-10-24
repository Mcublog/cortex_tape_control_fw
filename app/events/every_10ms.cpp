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
#include "app/mech/mech.hpp"
#include "app/ui/controls/Button.hpp"
//>>---------------------- Log control
#define LOG_MODULE_NAME e10ms
#define LOG_MODULE_LEVEL (3)
#include "log_libs.h"
//<<----------------------
//>>---------------------- Locals

// btn0 — REC, btn1 — PLAY, btn2 — STOP, btn3 — FF, btn4 — резерв, btn5 — REW.
static constexpr uint8_t kBtnNumber = 6;
static Button m_btns[kBtnNumber] = {Button(egde_t::EDGE_RAISING, 500, io_gpio_btn0),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn1),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn2),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn3),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn4),
                                    Button(egde_t::EDGE_RAISING, 500, io_gpio_btn5)};

// Запись запрошена, но начнётся только после перехода в стабильный STOP.
static bool m_rec_pending = false;

static bool mech_stable_stop()
{
    const Crp42602y &mech = Mech::instance();
    return (mech.mode() == mech::Mode::Stop) &&
           (mech.target_mode() == mech::Mode::Stop) &&
           (mech.transition_left() == 0);
}

static void cancel_pending_rec()
{
    m_rec_pending = false;
}

static void button_play()
{
    if (!Mech::ready())
        return;

    cancel_pending_rec();

    Crp42602y &mech = Mech::instance();
    mech::UserMode next = mech::UserMode::PlayFwd;

    // Повторное нажатие PLAY переключает направление.
    if (mech.user_mode() == mech::UserMode::PlayFwd)
        next = mech::UserMode::PlayRev;
    else if (mech.user_mode() == mech::UserMode::PlayRev)
        next = mech::UserMode::PlayFwd;

    if (!mech.request(next))
        LOG_WARNING("play: request rejected");
    else
        LOG_INFO("play: %s", (next == mech::UserMode::PlayRev) ? "rev" : "fwd");
}

static void button_stop()
{
    if (!Mech::ready())
        return;

    cancel_pending_rec();

    if (!Mech::instance().request(mech::UserMode::Stop))
        LOG_WARNING("stop: request rejected");
    else
        LOG_INFO("stop");
}

static void button_rew()
{
    if (!Mech::ready())
        return;

    cancel_pending_rec();

    if (!Mech::instance().request(mech::UserMode::FwindRev))
        LOG_WARNING("rew: request rejected");
    else
        LOG_INFO("rew");
}

static void button_ff()
{
    if (!Mech::ready())
        return;

    cancel_pending_rec();

    if (!Mech::instance().request(mech::UserMode::FwindFwd))
        LOG_WARNING("ff: request rejected");
    else
        LOG_INFO("ff");
}

static void button_rec()
{
    if (!Mech::ready())
        return;

    Crp42602y &mech = Mech::instance();

    if (m_rec_pending)
    {
        cancel_pending_rec();
        LOG_INFO("rec: cancelled");
        return;
    }

    if ((mech.user_mode() == mech::UserMode::RecFwd) ||
        (mech.user_mode() == mech::UserMode::RecRev))
    {
        LOG_INFO("rec: already recording");
        return;
    }

    if (mech_stable_stop())
    {
        if (!mech.request(mech::UserMode::RecFwd))
            LOG_WARNING("rec: rejected");
        return;
    }

    // Из активного режима сначала переходим в STOP.
    if (mech.request(mech::UserMode::Stop))
    {
        m_rec_pending = true;
        LOG_INFO("rec: stop before record");
    }
    else
    {
        LOG_WARNING("rec: stop rejected");
    }
}

static void process_pending_rec()
{
    if (!m_rec_pending)
        return;

    if (!Mech::ready())
    {
        cancel_pending_rec();
        return;
    }

    Crp42602y &mech = Mech::instance();

    if ((mech.mode() == mech::Mode::Halt) ||
        (mech.target_mode() == mech::Mode::Halt))
    {
        cancel_pending_rec();
        LOG_WARNING("rec: cancelled by halt");
        return;
    }

    if (!mech_stable_stop())
        return;

    if (mech.request(mech::UserMode::RecFwd))
        LOG_INFO("rec: started after stop");
    else
        LOG_WARNING("rec: rejected after stop");

    cancel_pending_rec();
}

static void handle_short_press(uint8_t index)
{
    switch (index)
    {
    case BTN0:
        button_rec();
        break;
    case BTN1:
        button_play();
        break;
    case BTN2:
        button_stop();
        break;
    case BTN3:
        button_ff();
        break;
    case BTN5:
        button_rew();
        break;
    default:
        // BTN4 пока не используется.
        break;
    }
}

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
    for (uint8_t i = 0; i < kBtnNumber; ++i)
    {
        if (m_btns[i].poll_short_press())
            handle_short_press(i);
    }

    process_pending_rec();
    button_process();
}
//<<----------------------
