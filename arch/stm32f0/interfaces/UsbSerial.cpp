/**
 * @file UsbSerial.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Stm32 USB serial device imlementation
 * @version 0.1
 * @date 2023-02-26
 *
 * @copyright Viacheslav mcublog (c) 2023
 *
 */
#include "UsbSerial.hpp"
#include "usbd_cdc_if.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME usbs
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

bool USBSerial::init(ios_ctl_t *ctl)
{
    Serial::init(ctl);
    return CDC_Get_Ctl(ctl);
}

bool USBSerial::write(uint8_t *data, uint32_t size)
{
    return CDC_Transmit_FS(data, size) == USBD_OK;
}