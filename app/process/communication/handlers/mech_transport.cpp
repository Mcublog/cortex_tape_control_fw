/**
 * @file mech_transport.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Высокоуровневые команды транспорта: стоп, воспроизведение, перемотка.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include "app/mech/types.hpp"
#include "app/proto/handlers.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME mtrn
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

/**
 * @brief Перевести действие протокола в пользовательский режим механизма.
 *
 * @param action код mech_action_t
 * @param mode результат, заполняется только при успехе
 * @return true, если действие известно
 */
static bool action2mode(uint32_t action, mech::UserMode &mode)
{
    switch (action)
    {
    case MECH_ACTION_STOP:
        mode = mech::UserMode::Stop;
        return true;
    case MECH_ACTION_PLAY_FWD:
        mode = mech::UserMode::PlayFwd;
        return true;
    case MECH_ACTION_PLAY_REV:
        mode = mech::UserMode::PlayRev;
        return true;
    case MECH_ACTION_REWIND_FWD:
        mode = mech::UserMode::FwindFwd;
        return true;
    case MECH_ACTION_REWIND_REV:
        mode = mech::UserMode::FwindRev;
        return true;
    default:
        return false;
    }
}

uint8_t *mech_transport(uint32_t id, const uint8_t *input, command_t *outcmd,
                        uint32_t size, uint32_t *outsize,
                        mech_mode_callback_t clb)
{
    const mech_transport_t *req = (const mech_transport_t *)input;
    command_ack_t *ack = (command_ack_t *)outcmd->data;

    mech::UserMode mode = mech::UserMode::Stop;

    ack->error = 0;

    if (clb == nullptr)
        ack->error = 3;
    else if (!action2mode(req->action, mode))
    {
        LOG_WARNING("unknown action: %u", (unsigned)req->action);
        ack->error = 1;
    }
    else if (!clb(static_cast<uint32_t>(mode)))
        ack->error = 2;

    outcmd->h.id = id;
    *outsize = sizeof(commands_head_t) + sizeof(command_ack_t);
    return (uint8_t *)outcmd;
}
