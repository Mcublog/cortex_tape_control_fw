#pragma once

#include <stdint.h>

typedef union {
    uint8_t d8;
    struct
    {
        uint8_t brake : 1;
        uint8_t tacho : 1;
        uint8_t sw1 : 1;
        uint8_t sw2 : 1;
        uint8_t sw3 : 1;
        uint8_t sw4 : 1;

        uint8_t reserved : 2;
    } f;
} inputs_t;

#ifdef __cplusplus
extern "C"
{
#endif

    inputs_t inputs_read();
    void inputs_print(inputs_t i);

#ifdef __cplusplus
}
#endif