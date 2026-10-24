/**
 * @file mech_get_state.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Чтение состояния механизма.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include <cstring>

#include "app/proto/handlers.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME mget
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

uint8_t *mech_get_state(uint32_t id, command_t *outcmd, uint32_t size,
                        uint32_t *outsize, mech_state_callback_t clb)
{
    mech_state_t *state = (mech_state_t *)outcmd->data;
    memset(state, 0, sizeof(mech_state_t));

    if (clb != nullptr)
        clb(state);

    outcmd->h.id = id;
    *outsize = sizeof(commands_head_t) + sizeof(mech_state_t);
    return (uint8_t *)outcmd;
}
