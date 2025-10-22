
#ifndef TIMER_MEASUREMENT_H
#define TIMER_MEASUREMENT_H

#include <stdint.h>

#define FUNC_PROFILE(f)                                                                  \
    do                                                                                   \
    {                                                                                    \
        tm_reset();                                                                      \
        f();                                                                             \
        uint32_t us = tm_get_expiried_time_us();                                         \
        LOG_INFO("time: %d us", us);                                                     \
    } while (0)

#ifdef __cplusplus
extern "C"
{
#endif

    void tm_accurate_checkout(void);
    void tm_reset(void);
    uint32_t tm_get_expiried_time_us(void);

    uint32_t tm_recalc_ticks_to_us(uint32_t ticks);

#ifdef __cplusplus
}
#endif

#endif // TIMER_MEASUREMENT_H