/**
 * @file itime.h
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Интерфейс для платформозависимого кода связанного с RTC
 * @version 0.1
 * @date 2024-12-08
 *
 * @copyright Viacheslav mcublog (c) 2024
 *
 */
#ifndef APP_UTILS_ITIME_H
#define APP_UTILS_ITIME_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    bool time_utils_is_need_to_init(void);

#ifdef __cplusplus
}
#endif

#endif // APP_UTILS_ITIME_H