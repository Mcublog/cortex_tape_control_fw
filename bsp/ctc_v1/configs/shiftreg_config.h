#pragma once

#include "main.h"
#include "spi.h"

#define OUTLATCH_PORT (nCS_GPIO_Port)
#define OUTLATCH_PIN (nCS_Pin)

// #define RESET_PORT ()
// #define RESET_PIN ()

// #define OE_PORT (DISP_RSE_GPIO_Port)
// #define OE_PIN (DISP_RSE_Pin)

#ifdef __cplusplus
extern "C"
{
#endif

#define SHIFTREG_SPI_HANDLE hspi2
extern SPI_HandleTypeDef SHIFTREG_SPI_HANDLE;

#ifdef __cplusplus
}
#endif
