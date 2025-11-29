/**
 * @file get_version.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief
 * @version 0.1
 * @date 2025-03-11
 *
 * @copyright Viacheslav mcublog (c) 2025
 *
 */
#include <cstddef>
#include <cstring>

#include "app/proto/handlers.h"
#include "app/utils/crc/crc32.h"
#include "app/version.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME gver
#define LOG_MODULE_LEVEL (3)
#include <debug/log_libs.h>
//<<----------------------

#define VERSION_BUF_SIZE (32)

typedef struct
{
    uint8_t version[VERSION_BUF_SIZE];
} command_version_req_t;

/**
 * @brief Get the version object
 *
 * @param outcmd
 * @param size
 * @param outsize
 * @return uint8_t*
 */
uint8_t *get_version(uint32_t id, command_t *outcmd, uint32_t size, uint32_t *outsize)
{
    command_version_req_t *req = (command_version_req_t *)outcmd->data;
    strncpy((char *)req->version, FW_VERSION, VERSION_BUF_SIZE);

    outcmd->h.id = id;
    *outsize = sizeof(commands_head_t) + sizeof(command_version_req_t);
    return (uint8_t *)outcmd;
}