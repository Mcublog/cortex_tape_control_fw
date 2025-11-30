/**
 * @file timer_mock.c
 * @author Viacheslav (Viacheslav@mcublog.ru)
 * @brief Timer mocking
 * @version 0.1
 * @date 2022-10-01
 *
 * @copyright Mcublog Copyright (c) 2022
 *
 */
#include <pthread.h>
#include <stdbool.h>

#include "app/system/icritical.h"
#include "app/utils/idelay.h"
#include "app/utils/itimer.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME timer_mock
#define LOG_MODULE_LEVEL (3)
#include "log_libs.h"
//<<----------------------

//>>---------------------- Local declarations
#define TIMER_MOCK_SYSTICK_PERIOD_MS (1)

static pthread_t _systick_thread_id;
static void *_sys_tick_timer_process(void *callback);
//<<----------------------

/**
 * @brief Systick timer emulation
 *
 */
static void *_sys_tick_timer_process(void *callback)
{
    timer_systick_callback_t run_callback = (timer_systick_callback_t)callback;
    while (true)
    {
        delay_ms(TIMER_MOCK_SYSTICK_PERIOD_MS);
        run_callback();
    }
}

/**
 * @brief Timer mocking initialization
 *
 */
void timer_init(timer_systick_callback_t systick_callback)
{
    if (_systick_thread_id != 0)
        return;
    pthread_create(&_systick_thread_id, NULL, _sys_tick_timer_process,
                   (void *)systick_callback);
}

void timer_irq_enable(void) { critical_exit(); };
void timer_irq_disable(void) { critical_enter(); };