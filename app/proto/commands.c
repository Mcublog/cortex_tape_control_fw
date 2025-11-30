/**
 * @file commands.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Command parser
 * @version 0.1
 * @date 2023-02-26
 *
 * @copyright Viacheslav mcublog (c) 2023
 *
 */
#include "commands.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME cmd
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

//>>---------------------- Local declaration
static const command_list_item_t *m_command_list;
//<<----------------------

//>>---------------------- Exported functions
/**
 * @brief Init command list
 *
 * @param command_list
 */
void command_parser_list_init(const command_list_item_t *command_list)
{
    m_command_list = command_list;
}

/**
 * @brief Find command handler and call
 *
 * @param command
 */
void command_parser(const command_t *command)
{
    for (int i = 0; i < CMDID_LAST; i++)
    {
        if (m_command_list[i].id != command->h.id)
            continue;
        if (m_command_list[i].handler == NULL)
        {
            LOG_WARNING("id: %#x not a have handler", command->h.id);
            return;
        }
        if (m_command_list[i].handler(command) == false)
            LOG_WARNING("id: %#x some trouble in handler", command->h.id);
        return;
    }
    LOG_WARNING("id: %#x not found", command->h.id);
    m_command_list[CMD_UNKNOWN].handler(command);
}
//<<----------------------