/**
 * @file application.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-23
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include "application.h"

#include "app/version.h"
#include "app/utils/idelay.h"

#include "arch/stm32f0/io/shiftregleds.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME app
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

/**
 * @brief
 *
 */
void application(void)
{
    LOG_INFO("Version: %s", FW_VERSION);
    shftregl_init();

    while (1)
    {
        shftregl_write(0x02);
        delay_ms(25);
        shftregl_write(0x00);
        delay_ms(250);
    }
}