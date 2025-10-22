
#ifndef CRC_8_H
#define CRC_8_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    uint8_t crc8(const void *data, size_t num_bytes);

#ifdef __cplusplus
}
#endif

#endif // CRC_8_H