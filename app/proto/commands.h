
#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    CMD_UNKNOWN = 0x00, // NOTE: must be in 0 index
    CMDID_GET_VERSION = 0x01,
    CMDID_SOLENOID_PULL = 0x02,
    CMDID_LAST
} command_id_t;

typedef struct
{
    uint32_t id;
} commands_head_t;

typedef struct
{
    commands_head_t h;
    uint8_t data[1];
} command_t;

typedef struct
{
    uint32_t error;
} command_ack_t;

typedef struct
{
    uint32_t pull_time_ms;
    uint32_t cooldown_ms;
} sollenoid_pull_t;

typedef struct command_list_item
{
    uint32_t id;
    bool (*handler)(const command_t *cmd);
} command_list_item_t;

typedef void(*command_callback_t)(void *data, uint32_t size);

#ifdef __cplusplus
extern "C" {
#endif

    void command_parser_list_init(const command_list_item_t *command_list);
    void command_parser(const command_t *command);

#ifdef __cplusplus
}
#endif

#endif // COMMANDS_H