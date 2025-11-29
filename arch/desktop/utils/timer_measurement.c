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
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include "app/utils/idelay.h"
#include "app/utils/itimer_measurement.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME tme
#define LOG_MODULE_LEVEL (3)
#include "log_libs.h"
//<<----------------------

//>>---------------------- Locals
// SYS_MULT calculate as example below
// SYS_MULT = (SystemCoreClock / 1000000U) = 168
static const uint32_t SYS_MULT = 16;
static const uint32_t CHECKOUT_PERIOD_MS = 10;
static const uint32_t CHECKOUT_MULT = 1000;
static const uint32_t DELTA_US = 1000;

static uint64_t m_last_us = 0;
//<<----------------------

static inline uint64_t get_current_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;
}

/**
 * @brief
 *
 */
void tm_accurate_checkout(void)
{

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
    m_last_us = get_current_us();
}

/**
 * @briefw
 *
 * @return uint32_t
 */
uint32_t tm_get_expiried_time_us(void)
{
    return get_current_us() - m_last_us;
}