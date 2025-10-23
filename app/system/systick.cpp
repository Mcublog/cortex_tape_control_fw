/**
 * @file systick.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-01-04
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "systick.h"
#include "libs/SoftwareTimer/SoftTimers.h"

void systick_handler(void)
{
    Timer_Update();
}