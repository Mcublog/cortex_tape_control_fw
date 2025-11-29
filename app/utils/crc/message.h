
#ifndef APP_UTILS_CRC_MESSAGE_H
#define APP_UTILS_CRC_MESSAGE_H

#include "app/io/serial/types.h"

#ifdef __cplusplus
extern "C" {
#endif

    uint32_t message_crc(const ios_message_t *m);

#ifdef __cplusplus
}
#endif

#endif // APP_UTILS_CRC_MESSAGE_H