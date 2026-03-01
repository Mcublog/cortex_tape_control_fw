/**
 * @file hwtest_analog_scale.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Viacheslav mcublog (c) 2026
 *
 */
#include <sys/time.h>

#include "app/application.h"
#include "app/io/igpio.h"
#include "app/utils/idelay.h"
#include "app/version.h"

#include "main.h"
#include "tim.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME app
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

extern TIM_HandleTypeDef htim2;

static void change_percent(uint16_t percent)
{
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);

    TIM_OC_InitTypeDef sConfigOC = {};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = percent;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

/**
 * @brief
 *
 */
void application(void)
{
    LOG_INFO("Version: %s", FW_VERSION);

    // sw3_init_as_output();
    MX_TIM2_Init();

    uint16_t percent = 50;
    bool dir = true;

    while (1)
    {
        change_percent(percent);
        if (dir)
            percent += 50;
        else
            percent -= 50;

        if (percent > 900 || percent <= 50)
        {
            dir ^= true;
        }
        delay_ms(100);
    }
}