/**
 * @file mech_set_mode.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Запрос режима механизма.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include <cstring>

#include "app/proto/handlers.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME mset
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

uint8_t *mech_set_mode(uint32_t id, const uint8_t *input, command_t *outcmd,
                       uint32_t size, uint32_t *outsize,
                       mech_mode_callback_t clb)
{
    const mech_set_mode_t *req = (const mech_set_mode_t *)input;
    command_ack_t *ack = (command_ack_t *)outcmd->data;

    ack->error = 0;

    if (clb == nullptr)
        ack->error = 3;
    else if (req->mode > 6)
        ack->error = 1;
    else if (!clb(req->mode))
        ack->error = 2;

    outcmd->h.id = id;
    *outsize = sizeof(commands_head_t) + sizeof(command_ack_t);
    return (uint8_t *)outcmd;
}
