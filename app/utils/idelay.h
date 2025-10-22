
#ifndef IDELAY_H
#define IDELAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    void delay_ms(uint32_t ms);
    void delay_us(uint16_t us);

#ifdef __cplusplus
}
#endif

#endif // IDELAY_H