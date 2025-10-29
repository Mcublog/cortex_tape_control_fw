/**
 * @file hwtest_shiftreg.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-22
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <sys/time.h>

#include "app/application.h"
#include "app/utils/idelay.h"
#include "app/version.h"

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


    uint8_t value = 1;
    while (1)
    {
        for (uint32_t i = 0; i < 1000; i++)
            shftregl_write(value);
        // delay_ms(1000);
        value = (value << 1);
        if (value == 0)
            value = 1;
    }
}