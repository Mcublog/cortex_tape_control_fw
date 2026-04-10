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
static void power_set(bool on)
{
    LOG_INFO("power_set: %d", on);
    io_gpio_power(on);
}

static void solenoid_pull_static(bool s)
{
    shftregl_write(s ? 1 : 0);
    io_gpio_solenoid(s);
}

static void solenoid_pull(uint16_t pulse_ms)
{
    io_gpio_solenoid(true);
    shftregl_write(1);
    delay_ms(pulse_ms);
    io_gpio_solenoid(false);
    shftregl_write(0);
}

static void stop(bool power_off)
{
    if (power_off)
    {
        power_set(true);
        delay_ms(100);
    }
    LOG_INFO("stop");
    solenoid_pull(225);
    if (power_off)
    {
        power_set(false);
    }
    delay_ms(1000);
}

void gear_func_sequence(bool head_dir_is_a, bool lift_head, bool reel_fwd)
{
    // Function sequence has 190 degree of function gear to rotate in 400 ms
    // Timing definitions (milliseconds) (All values are set experimentally)
    constexpr uint32_t tInitS     = 0;           // Unhook the function gear
    constexpr uint32_t tInitE     = 20;
    constexpr uint32_t tHeadDirS  = tInitE;      // Term to determine direction of head and pinch roller
    constexpr uint32_t tHeadDirE  = 100;
    constexpr uint32_t tLiftHeadS = 150;         // Term to lift head / evacuate head
    constexpr uint32_t tLiftHeadE = 300;
    constexpr uint32_t tReelS     = tLiftHeadE;  // Term to determine reel direction
    constexpr uint32_t tReelE     = 400;

    // Be careful about the consistency of pinch roller direction and reel direction,
    //  otherwise they could pull to opposite directions and give unexpected extension stress to the tape

    power_set(true);
    delay_ms(100);

    solenoid_pull_static(true);
    delay_ms(tInitE);
    solenoid_pull_static(!head_dir_is_a);
    delay_ms(tHeadDirE - tInitE);
    solenoid_pull_static(false);
    delay_ms(tLiftHeadS - tHeadDirE);
    solenoid_pull_static(lift_head);
    delay_ms(tLiftHeadE - tLiftHeadS);
    solenoid_pull_static(reel_fwd);
    delay_ms(tReelE - tLiftHeadE);
    solenoid_pull_static(false);
    delay_ms(20);  // additional margin

    // power_set(false);
    // delay_ms(100);
}

static void play_a()
{
    LOG_INFO("play_a");
    gear_func_sequence(false, true, false);
}

static void rew_a()
{
    LOG_INFO("rew_a");
    gear_func_sequence(false, false, false);
}

static void rew_b()
{
    LOG_INFO("rew_b");
    gear_func_sequence(true, false, true);
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
    power_set(false);
    while (1)
    {
        stop(true);
        delay_ms(2500);
        LOG_INFO("----- new cycle ----");
        play_a();
        // gear_func_sequence(true, false, true);
        // solenoid_pull(50);
        // gear_func_sequence(true, true, true);
        // play();
        delay_ms(5000);
        stop(false);
        // solenoid_pull(150);
        rew_b();
        // stop();
        // delay_ms(100);
        // // stop();
        // rew_a();
        delay_ms(5000);
        // stop(false);

        LOG_INFO("--------------");
    }
}