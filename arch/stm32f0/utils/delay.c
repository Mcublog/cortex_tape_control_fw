/**
 * @file delay.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2024-12-08
 *
 * @copyright Viacheslav mcublog (c) 2024
 *
 */
#include "app/utils/idelay.h"
#include "app/utils/itick_timer.h"

#include "main.h"
#include "tim.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME delay
#if defined(NDEBUG)
#define LOG_MODULE_LEVEL (0)
#else
#define LOG_MODULE_LEVEL (3)
#endif
#include "libs/debug/log_libs.h"
//<<----------------------

//>>---------------------- Public

/**
 * @brief
 *
 * @param ms
 */
void delay_ms(uint32_t ms)
{
  HAL_Delay(ms);
}

void delay_us(uint16_t us)
{
  tt_delay_us(&htim6, us);
}
//<<----------------------