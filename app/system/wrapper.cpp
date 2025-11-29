/**
 * @file wrapper.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief C wrappers for cpp some cpp code
 * @version 0.1
 * @date 2023-12-17
 *
 * @copyright Mcublog Copyright (c) 2023
 *
 */
#include "wrapper.h"

#include "app/proto/cobs/Parser.hpp"
#include "app/system/system.h"

bool cobsw_is_message_received(void)
{
    return isystem()->cobs_parser()->is_message_received();
}

uint32_t cobsw_irq_handler(ios_chunk_t *chunk)
{
    return isystem()->cobs_parser()->irq_handler(chunk);
}
