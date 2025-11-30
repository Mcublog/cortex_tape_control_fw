
#ifndef APP_SYSTEM_SYSTEM_H
#define APP_SYSTEM_SYSTEM_H

#include "app/interfaces/System.hpp"

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Link function to actual system implemenation
     *
     * @return System* it is pointer to actual system implementation
     * for example: desktop emulator or hw on stm32
     */
    System *isystem();

#ifdef __cplusplus
}
#endif

#endif // APP_SYSTEM_SYSTEM_H