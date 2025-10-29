#ifndef ITICK_TIMER_H
#define ITICK_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void tt_init(void *header);
    void tt_delay_us(void *header, uint16_t us);
    void tt_reset(void *header);
    uint16_t tt_read(void *header);

#ifdef __cplusplus
}
#endif

#endif // ITICK_TIMER_H