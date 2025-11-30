/**
 * @file solenoid_pull.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-11-30
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <cstring>

#include "app/proto/handlers.h"
#include "app/utils/crc/crc32.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME solpull
#if defined(NDEBUG)
#define LOG_MODULE_LEVEL (0)
#else
#define LOG_MODULE_LEVEL (3)
#endif
#include "log_libs.h"
//<<----------------------
uint8_t *solenoid_pull(uint32_t id, const uint8_t *input, command_t *outcmd,
                               uint32_t size, uint32_t *outsize, command_callback_t clb)
{
    sollenoid_pull_t *req = (sollenoid_pull_t *)input;

    sollenoid_pull_t s = {};

    s.pull_time_ms = req->pull_time_ms;
    s.cooldown_ms = req->cooldown_ms;

    if (clb)
    {
        clb((void*)&s, sizeof(s));
    }

    command_ack_t *ack = (command_ack_t *)outcmd->data;
    ack->error = 0;

    outcmd->h.id = id;
    *outsize = sizeof(commands_head_t) + sizeof(command_ack_t);
    return (uint8_t *)outsize;
}