#pragma once

#include <stdbool.h>

enum
{
    BTN0 = 0,
    BTN1,
    BTN2,
    BTN3,
    BTN4,
    BTN5
};

#ifdef __cplusplus
extern "C"
{
#endif

    void io_gpio_solenoid(bool on);
    bool io_gpio_btn0(void);
    bool io_gpio_btn1(void);

    bool io_gpio_brake_sens(void);
    bool io_gpio_sw1(void);
    bool io_gpio_sw2(void);
    bool io_gpio_sw3(void);
    bool io_gpio_sw4(void);
    bool io_gpio_tacho(void);

#ifdef __cplusplus
}
#endif
