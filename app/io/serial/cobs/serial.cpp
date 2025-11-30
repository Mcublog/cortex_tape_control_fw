/**
 * @file io_serial.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2023-02-26
 *
 * @copyright Viacheslav mcublog (c) 2023
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <cstdlib>
#include <cstring>

// #include "app/system/common.h"
#include "app/io/serial/cobs/serial.h"
#include "app/interfaces/Serial.hpp"
#include "libs/cobs-c/cobsr.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME ios
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

//>>---------------------- Local declaration
static ios_ctl_t m_ctl = {};
static uint8_t m_pos = 0;
static bool m_msg_ready = false;
static Serial *m_sdev = nullptr;

static const char BINARY_END_CHAR = '\0';
//<<----------------------

//>>---------------------- Local definitions

//<<----------------------

//>>---------------------- Exported function
/**
 * @brief
 *
 * @param ctl -- control
 * @return true
 * @return false
 */
bool ios_serial_init(void *dev)
{
    m_ctl.irq_handler = ios_irq_handler;
    m_sdev = reinterpret_cast<Serial*>(dev);
    return m_sdev->init(&m_ctl);
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool ios_is_message_received(void)
{
    return m_msg_ready;
}

/**
 * @brief
 *
 * @param message
 * @return true
 * @return false
 */
ios_message_t *ios_read_message(void)
{
    m_msg_ready = false;

    cobsr_decode_result result =
        cobsr_decode(m_ctl.out.data, m_ctl.out.limit,
                     m_ctl.in.data, (--m_pos));
    m_pos = 0;
    if (result.status != COBSR_DECODE_OK)
    {
        LOG_ERROR("result decode: %d", result.status);
        return NULL;
    }
    LOG_INFO("rx: size: %d", ((ios_message_t*)m_ctl.out.data)->head.size);

    return (ios_message_t*)m_ctl.out.data;
}

uint8_t *ios_get_output_buffer(uint32_t *limit)
{
    *limit = m_ctl.in.limit;
    return (uint8_t *)m_ctl.in.data;
}

/**
 * @brief
 *
 * @param message
 * @return true
 * @return false
 */
bool ios_write_message(uint8_t *message, uint32_t size)
{
    ios_message_t *msg = (ios_message_t *)m_ctl.in.data;
    msg->head.size = size;
    cobsr_encode_result result =
        cobsr_encode(m_ctl.out.data, m_ctl.out.limit, m_ctl.in.data, msg->head.size);
    if (result.status != COBSR_ENCODE_OK)
    {
        LOG_ERROR("result encode: %d", result.status);
        return false;
    }
    if (msg->head.size == (m_ctl.out.limit - 1))
    {
        LOG_ERROR("size to big");
        return false;
    }
    m_ctl.out.data[msg->head.size++] = BINARY_END_CHAR;
    return m_sdev->write(m_ctl.out.data, msg->head.size);
}

/**
 * @brief
 *
 */
uint32_t ios_irq_handler(ios_chunk_t *chunk)
{
    if (m_msg_ready)
        return m_pos;
    if (m_ctl.in.data == NULL || m_ctl.out.data == NULL)
    {
        LOG_ERROR("ios ctl not initialized");
        return m_pos;
    }
    if ((m_pos + chunk->size) >= m_ctl.in.limit)
    {
        LOG_ERROR("received buffer is overrun");
        m_pos = 0;
        return m_pos;
    }
    memcpy(&m_ctl.in.data[m_pos], chunk->data, chunk->size);
    m_pos += chunk->size;
    m_msg_ready = (m_ctl.in.data[m_pos - 1] == BINARY_END_CHAR);
    return m_pos;
}
//<<----------------------
