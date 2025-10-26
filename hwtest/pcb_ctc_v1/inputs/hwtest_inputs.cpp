/**
 * @file hwtest_inputs.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-10-26
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

    while (1)
    {

    }
}