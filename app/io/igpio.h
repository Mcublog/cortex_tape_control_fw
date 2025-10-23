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

#ifdef __cplusplus
}
#endif
