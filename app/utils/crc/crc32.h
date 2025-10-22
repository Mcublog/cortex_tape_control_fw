#ifndef APP_UTILS_CRC32_H
#define APP_UTILS_CRC32_H

#define CRC32_LENGHT (4)

#ifdef __cplusplus
extern "C"
{
#endif

    unsigned int xcrc32(const unsigned char *buf, int len, unsigned int init);

#ifdef __cplusplus
}
#endif

#endif // APP_UTILS_CRC32_H
