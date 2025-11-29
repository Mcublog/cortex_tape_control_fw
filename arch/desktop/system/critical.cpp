/**
 * @file critical.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-07
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <mutex>

#include "app/system/icritical.h"

static std::mutex m_mtx = {};

void critical_enter(void)
{
    m_mtx.lock();
}

void critical_exit(void)
{
    m_mtx.unlock();
}
