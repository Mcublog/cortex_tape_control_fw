
#ifndef APP_PROTO_HANDLERS_H
#define APP_PROTO_HANDLERS_H

#include <stdint.h>
#include "commands.h"

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t *unk_command_handler(uint32_t id, command_t *outcmd, uint32_t size,
                                 uint32_t *outsize);

    uint8_t *get_version(uint32_t id, command_t *outcmd, uint32_t size,
                         uint32_t *outsize);

    uint8_t *current_values(uint32_t id, command_t *outcmd, uint32_t size,
                         uint32_t *outsize);

    uint8_t *settings_get_handler(uint32_t id, command_t *outcmd, uint32_t size,
                                  uint32_t *outsize);
    uint8_t *settings_set_handler(uint32_t id, const uint8_t *input, command_t *outcmd,
                                  uint32_t size, uint32_t *outsize, command_callback_t c);

    uint8_t *state_set_handler(uint32_t id, const uint8_t *input, command_t *outcmd,
                               uint32_t size, uint32_t *outsize, command_callback_t clb);

#ifdef __cplusplus
}
#endif

#endif // APP_PROTO_HANDLERS_H