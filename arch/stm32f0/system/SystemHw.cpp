/**
 * @file SystemEmu.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Implementaion of Emulator system
 * @version 0.1
 * @date 2023-12-16
 *
 * @copyright Mcublog Copyright (c) 2023
 *
 */
#include "SystemHw.hpp"

#include "app/proto/cobs/Parser.hpp"
#include "app/utils/idelay.h"

#include "arch/stm32f0/interfaces/UsbSerial.hpp"

#include "main.h"

#include "stm32f0xx_ll_pwr.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME hwsys
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//>>---------------------- Locals
static USBSerial m_sdev = USBSerial();
static CobsParser m_cobsp = CobsParser();
//<<----------------------
void SystemHW::init()
{
    m_cobsp.init(&m_sdev);
}
/**
 * @brief Print system type in log
 *
 */
void SystemHW::what(void)
{
    LOG_INFO("it is HW System");
}

Serial *SystemHW::communication_serial()
{
    return (Serial *)&m_sdev;
}
/**
 * @brief
 *
 * @return CobsParser*
 */
CobsParser *SystemHW::cobs_parser()
{
    return &m_cobsp;
}

/**
 * @brief Go to stanby mode
 *
 * @return true need to return from app
 * @return false
 */
bool SystemHW::go_to_stanby(void)
{
    return false;
}

bool SystemHW::prepare_to_sleep()
{
    // Убрать систик
    HAL_SuspendTick();
    return false;
}