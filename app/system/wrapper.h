
#ifndef APP_SYSTEM_WRAPPER_H
#define APP_SYSTEM_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>

#include "app/io/serial/types.h"

#ifdef __cplusplus
extern "C" {
#endif

    bool cobsw_is_message_received(void);
    uint32_t cobsw_irq_handler(ios_chunk_t *chunk);

#ifdef __cplusplus
}
#endif

#endif // APP_SYSTEM_WRAPPER_H