/**
 * @file message.c
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-03-15
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <stddef.h>

#include "message.h"
#include "crc32.h"

uint32_t message_crc(const ios_message_t *m)
{
    uint8_t *content = &((uint8_t *)m)[offsetof(ios_header_t, channel)];
    uint32_t size = m->head.size - sizeof(m->head.crc);
    return xcrc32((const unsigned char *)content, size, 0xFFFFFFFF);
}