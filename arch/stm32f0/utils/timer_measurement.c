/**
 * @file timer_measurement.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief timer_measurement module
 * @version 0.1
 * @date 2023-08-26
 *
 * @copyright Viacheslav mcublog (c) 2023
 *
 */
#include <stdint.h>
#include <stdbool.h>

#include "tim.h"
#include "main.h"
#include "app/utils/itick_timer.h"
#include "app/utils/idelay.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME tm
#define LOG_MODULE_LEVEL (3)
#include "log_libs.h"
//<<----------------------

//>>---------------------- Global
extern TIM_HandleTypeDef htim7;
//<<----------------------

//>>---------------------- Locals
// SYS_MULT calculate as example below
// SYS_MULT = (SystemCoreClock / 1000000U) = 168
static const uint32_t CHECKOUT_PERIOD_MS = 10;
static const uint32_t CHECKOUT_MULT = 1000;
static const uint32_t DELTA_US = 1000;
//<<----------------------

/**
 * @brief
 *
 */
void tm_accurate_checkout(void)
{
    tt_reset(&htim7);
    tt_reset(&htim7);
    delay_ms(CHECKOUT_PERIOD_MS);
    uint16_t us = tt_read(&htim7);
    uint32_t dmin = CHECKOUT_PERIOD_MS * CHECKOUT_MULT - DELTA_US;
    uint32_t dmax = CHECKOUT_PERIOD_MS * CHECKOUT_MULT + DELTA_US;
    if ((us >= dmin && us <= dmax) == false)
    {
        LOG_ERROR("accurate FAILED: %ld us != %ld", us,
                  CHECKOUT_PERIOD_MS * CHECKOUT_MULT);
        while (1)
        {
        };
    }
    int32_t error = (int32_t)us - (int32_t)(CHECKOUT_PERIOD_MS * CHECKOUT_MULT);
    if (error < 0)
        error *=(-1);
    LOG_INFO("accurate OK: %ld us == %ld error: %d.%d%%", us,
             CHECKOUT_PERIOD_MS * CHECKOUT_MULT,
             error / 100, error % 100);
}

/**
 * @brief
 *
 * @param ticks
 * @return uint32_t
 */
uint32_t tm_recalc_ticks_to_us(uint32_t ticks)
{
    return ticks;
}

/**
 * @brief
 *
 */
void tm_reset(void)
{
    tt_reset(&htim7);
}

/**
 * @briefw
 *
 * @return uint32_t
 */
uint32_t tm_get_expiried_time_us(void)
{
    return tt_read(&htim7);
}