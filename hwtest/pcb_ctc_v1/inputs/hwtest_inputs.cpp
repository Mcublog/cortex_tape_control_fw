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
#include "app/io/inputs.h"
#include "app/utils/idelay.h"
#include "app/version.h"

#include "bsp/ctc_v1/ui/ShiftRegInd.hpp"
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

    ShiftRegInd ind = {};
    ind.init();
    ind.set_bit((uint8_t)0, true);
    inputs_t i = inputs_read();
    LOG_INFO("inputs: 0x%02x", i.d8);
    inputs_print(i);
    while (1)
    {
        i = inputs_read();
        ind.set_inputs(i);
        delay_ms(10);
    }
}