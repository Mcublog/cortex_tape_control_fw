
#ifndef APP_IO_SERIAL_COBS_SERIAL_H
#define APP_IO_SERIAL_COBS_SERIAL_H

#include <stdbool.h>
#include <stdint.h>

#include "app/io/serial/types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    bool ios_serial_init(void *dev);

    bool ios_is_message_received(void);

    ios_message_t *ios_read_message(void);

    uint8_t *ios_get_output_buffer(uint32_t *limit);
    bool ios_write_message(uint8_t *message, uint32_t size);

    uint32_t ios_irq_handler(ios_chunk_t *chunk);

#ifdef __cplusplus
}
#endif

#endif // APP_IO_SERIAL_COBS_SERIAL_H