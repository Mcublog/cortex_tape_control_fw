/**
 * @file Indication.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-26
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "Indication.hpp"

#include "app/utils/idelay.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME indi
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------
void Indication::init()
{
    ll_init();
    m_state = 0xFF;
    ll_write(m_state);
    delay_ms(50);
    m_state = 0x00;
    ll_write(m_state);
}

void Indication::set_bit(uint8_t bit_number, bool state)
{
    if (state)
        m_state |= (1 << bit_number);
    else
        m_state &= ~(1 << bit_number);
    ll_write(m_state);
}

void Indication::set_inputs(inputs_t i)
{
    m_state = i.d8;
    ll_write(m_state);
}