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

#include "app/io/igpio.h"
#include "app/process/communication/process.hpp"
#include "app/system/system.h"
#include "app/utils/idelay.h"
#include "app/version.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME app
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

static void solenoid_pull(uint16_t pulse_ms)
{
    io_gpio_solenoid(true);
    isystem()->indication()->set_bit(0, 1);
    delay_ms(pulse_ms);
    io_gpio_solenoid(false);
    isystem()->indication()->set_bit(0, 0);
}


static void on_solenoid_pull(void *data, uint32_t size)
{
    const sollenoid_pull_t cmd = *((const sollenoid_pull_t *)data);

    LOG_INFO("pull param: pull ms: %d cooldown ms: %d", cmd.pull_time_ms,
             cmd.cooldown_ms);

    solenoid_pull(cmd.pull_time_ms);
    delay_ms(cmd.cooldown_ms);
}

/**
 * @brief
 *
 */
void application(void)
{
    LOG_INFO("Version: %s", FW_VERSION);

    System *sys = isystem();
    if (sys == nullptr)
    {
        LOG_ERROR("sys is not ready");
        System::infitite_loop();
    }
    sys->what();
    sys->init();

    isystem()->indication()->init();

    Communication::callback_attach_solenoid_pull((command_callback_t)on_solenoid_pull);

    while (1)
    {
        Communication::process();
    }
}