/**
 * @file process.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2023-12-17
 *
 * @copyright Mcublog Copyright (c) 2023
 *
 */
#include <cstring>
#include <ctime>

#include "process.hpp"

#include "app/io/igpio.h"
#include "app/proto/cobs/Parser.hpp"
#include "app/proto/handlers.h"
#include "app/system/system.h"
#include "app/utils/crc/message.h"
#include "app/utils/idelay.h"
#include "app/version.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME comm
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//>>---------------------- Locals
CobsParser *m_parser = nullptr;
command_callback_t m_settings_changed_clb = nullptr;
command_callback_t m_ininital_state_changed_clb = nullptr;

static bool command_handler(const command_t *command)
{
    uint32_t limit = 0, version = 0, channel = 0, outsize = 0;
    uint8_t *ack = nullptr;
    command_t *output = reinterpret_cast<command_t *>(
        isystem()->cobs_parser()->get_output_buffer(&limit));

    LOG_INFO("handle: id: %#x version: %#x", command->h.id, version);
    if (command->h.id == command_id_t::CMDID_GET_VERSION)
        ack = get_version(command->h.id, output, limit, &outsize);
    // else if (command->h.id == command_id_t::CMDID_GET_CURRENT_VALUES)
    //     ack = current_values(command->h.id, output, limit, &outsize);
    // else if (command->h.id == command_id_t::CMDID_GET_SETTINGS)
    //     ack = settings_get_handler(command->h.id, output, limit, &outsize);
    // else if (command->h.id == command_id_t::CMDID_SET_SETTINGS)
    //     ack = settings_set_handler(command->h.id, command->data, output, limit, &outsize,
    //                                m_settings_changed_clb);
    // else if (command->h.id == command_id_t::CMDID_SET_STATE)
    //     ack = state_set_handler(command->h.id, command->data, output, limit, &outsize,
    //                             m_ininital_state_changed_clb);

    if (!ack)
        ack = unk_command_handler(command->h.id, output, limit, &outsize);

    isystem()->cobs_parser()->write_message(version, channel, ack, outsize);
    return true;
}

static const command_list_item_t m_command_list[CMDID_LAST] = {
    {CMD_UNKNOWN, command_handler}, // NOTE: must be in 0 index
    {CMDID_GET_VERSION, command_handler},
    {CMDID_SET_SOLENOID, command_handler},
};

//<<----------------------

bool Communication::process()
{
    m_parser = isystem()->cobs_parser();

    command_parser_list_init((const command_list_item_t *)&m_command_list);

    if (m_parser->is_message_received() == false)
    {
        // delay_ms(1);
        return false;
    }

    ios_message_t *msg = m_parser->read_message();
    uint32_t crc32 = message_crc(msg);

    LOG_INFO("msg->head.crc: 0x%x == 0x%x", msg->head.crc, crc32);
    command_parser((const command_t *)msg->data);

    return false;
}
