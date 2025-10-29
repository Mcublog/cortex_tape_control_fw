
#ifndef APP_IO_DEBOUNCE_H
#define APP_IO_DEBOUNCE_H

#include <stdbool.h>
#include <stdint.h>

#include "app/io/types.h"
#include "libs/Ring-Buffer/ringbuffer.h"

#define DEBOUNCE_SIZE (8)

typedef struct debounce
{
    uint8_t buffer[DEBOUNCE_SIZE];
    ring_buffer_t rb;
    uint16_t state_counter;
    bool last_state;
} debouncer_t;


#ifdef __cplusplus
extern "C" {
#endif

    void dbnc_init(debouncer_t *db, bool state);
    void dbnc_set_current_state(debouncer_t *db, bool state);
    bool dbnc_get_state(const debouncer_t *db);
    egde_t dbnc_get_edge(debouncer_t *db);
    bool dbnc_is_ready(const debouncer_t *db);
    uint16_t dgnc_state_counter(const debouncer_t *db);
    void dgnc_reset_state_counter(debouncer_t *db);

#ifdef __cplusplus
}
#endif

#endif // APP_SENSORS_DEBOUNCE_H