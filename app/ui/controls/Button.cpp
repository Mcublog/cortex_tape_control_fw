/**
 * @file Button.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-01-05
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "Button.hpp"
//>>---------------------- Log control
#define LOG_MODULE_NAME     btn
#define LOG_MODULE_LEVEL    (3)
#include <debug/log_libs.h>
//<<----------------------
void Button::init()
{
    dbnc_init(&m_db, m_poll_pin());
}

void Button::process()
{
    bool state = m_poll_pin();
    dbnc_set_current_state(&m_db, state);
    press_handling();
}

void Button::press_handling()
{
    const egde_t e = dbnc_get_edge(&m_db);
    const uint16_t cnt = dgnc_state_counter(&m_db);

    const egde_t long_press_edge =
        m_active_edge == EDGE_RAISING ? EDGE_HIGH_PLATO : EDGE_LOW_PLATO;

    const egde_t reset_edge =
        m_active_edge == EDGE_RAISING ? EDGE_FALLING : EDGE_RAISING;

    // зафиксировали нажатие кнопки
    if (e == m_active_edge)
    {
        m_short_press = false;
        m_pressed = true;
    }
    else if (e == reset_edge && m_pressed) // кнопку отпустили и таймер не сработал
    {
        LOG_DEBUG("short press");
        m_short_press = true;
    }
    else if (e == long_press_edge && cnt == m_long_press_cnt && m_pressed)
    {
        LOG_DEBUG("long press");
        m_pressed = false;
        m_short_press = false;
        m_long_press = true;
    }
    else if (e == reset_edge)
    {
        m_pressed = false;
    }
}

/**
 * @brief Сбрасывает состояние, послу успешного опроса
 *
 * @return true
 * @return false
 */
bool Button::poll_short_press()
{
    return press_poll(m_short_press);
}

/**
 * @brief Сбрасывает состояние, послу успешного опроса
 *
 * @return true
 * @return false
 */
bool Button::poll_long_press()
{
    return press_poll(m_long_press);
}
