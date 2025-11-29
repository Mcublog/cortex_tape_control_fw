/**
 * @file delay.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-01-07
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <unistd.h>

#include "app/utils/idelay.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME     edelay
#define LOG_MODULE_LEVEL    (3)
#include "log_libs.h"
//<<----------------------

#define MS_MULT (1000)

/**
 * @brief
 *
 * @param ms
 */
void delay_ms(uint32_t ms)
{
    usleep(ms * MS_MULT);
}

void delay_long_us(uint32_t us)
{
    usleep(us);
}

/**
 * @brief
 *
 * @param us
 */
void delay_us(uint16_t us)
{
    usleep(us);
}
