/**
 * @file hwtest_solenoid.cpp
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
#include "app/io/igpio.h"
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
    shftregl_write(0);

    uint32_t pulse_delay = 500;

    while (1)
    {
        delay_ms(pulse_delay);
        io_gpio_solenoid(false);
        delay_ms(pulse_delay);
        io_gpio_solenoid(true);
        delay_ms(1000);
    }
}