/**
 * @file main.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-01-07
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <cstdio>

#include "app/application.h"
#include "app/system/systick.h"
#include "app/utils/idelay.h"
#include "app/utils/itimer.h"
#include "io_emu/io_mock.h"
#include "libs/SoftwareTimer/SoftTimers.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME main
#define LOG_MODULE_LEVEL (3)
#include <log_libs.h>
//<<----------------------

//>>---------------------- Global
int main(void)
{
    iomock_init();
    timer_init(systick_handler);

    const stimer_init_ctx_t m_timer_ctx = {.enable_irq = timer_irq_enable,
                                           .disable_irq = timer_irq_disable};

    Timer_Init(&m_timer_ctx);

    while (1)
    {
        application();
    }
}
//<<----------------------
