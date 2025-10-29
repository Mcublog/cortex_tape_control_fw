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

static void solenoid_pull(uint16_t pulse_ms)
{
    io_gpio_solenoid(true);
    shftregl_write(1);
    delay_ms(pulse_ms);
    io_gpio_solenoid(false);
    shftregl_write(0);
}

static void stop()
{
    LOG_INFO("stop");
    solenoid_pull(200);
    delay_ms(1000);
}

static void play()
{
    LOG_INFO("play");
    solenoid_pull(400);
    delay_ms(1000);
}
/**
 * @brief
 *
 */
void application(void)
{
    LOG_INFO("Version: %s", FW_VERSION);
    shftregl_init();
    shftregl_write(0);

    while (1)
    {
        LOG_INFO("----- new cycle ----");
        play();
        delay_ms(5000);
        stop();
        delay_ms(2500);
        LOG_INFO("--------------");
    }
}