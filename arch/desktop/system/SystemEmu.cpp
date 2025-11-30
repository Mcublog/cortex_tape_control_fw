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
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "SystemEmu.hpp"

#include "app/proto/cobs/Parser.hpp"
#include "app/utils/idelay.h"

#include "arch/desktop/interfaces/Sdev.hpp"

#include "bsp/desktop/ui/ShiftRegInd.hpp"
//>>---------------------- Log control
#define LOG_MODULE_NAME esys
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

//>>---------------------- Locals
#define INPUT_BUFFER_SIZE   (256)
#define OUTPUT_BUFFER_SIZE  (256)

static void *communication_reading(void*);

static SDevice m_sdev = SDevice("/dev/ttyS11", communication_reading);
static CobsParser m_cobsp = CobsParser();
static ShiftRegInd m_indication = ShiftRegInd();

static void read_process(SDevice *ser)
{
    while (1)
    {
        uint8_t byte = 0;
        long size = read(ser->m_io_stream, &byte, 1);
        if (size <= 0)
        {
            delay_ms(1);
            continue;
        }
        ios_chunk_t data = {&byte, 1};
        ser->irq_handler(&data);
    }
}

static void *communication_reading(void*)
{
    read_process(&m_sdev);
    return nullptr;
}
//<<----------------------
void SystemEmu::init()
{
    m_cobsp.init(&m_sdev);
}
/**
 * @brief Print system type in log
 *
 */
void SystemEmu::what(void)
{
    LOG_INFO("it is Emulator System");
}

Serial *SystemEmu::communication_serial()
{
    return &m_sdev;
}

Indication *SystemEmu::indication()
{
    return &m_indication;
}
/**
 * @brief
 *
 * @return CobsParser*
 */
CobsParser *SystemEmu::cobs_parser()
{
    return &m_cobsp;
}
