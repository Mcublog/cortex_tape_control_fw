/**
 * @file unknown.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-03-11
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <cstring>

#include "app/proto/handlers.h"
#include "app/utils/crc/crc32.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME unkh
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

typedef struct
{
    uint32_t status;
}unk_command_ack_t;

uint8_t *unk_command_handler(uint32_t id, command_t *outcmd, uint32_t size,
                             uint32_t *outsize)
{
    unk_command_ack_t *payload = (unk_command_ack_t *)outcmd->data;
    payload->status = 1;

    outcmd->h.id = id;
    *outsize = sizeof(commands_head_t) + sizeof(unk_command_ack_t);
    return (uint8_t *)outcmd;
}
