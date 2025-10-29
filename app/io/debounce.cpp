/**
 * @file debounce.c
 * @author Viacheslav (v.konovalov@crystals.ru)
 * @brief
 * @version 0.1
 * @date 2024-03-12
 *
 * @copyright CSI Copyright (c) 2024
 *
 */
#include <cassert>
#include <cstring>

#include "app/io/debounce.h"
//>>---------------------- Log control
#define LOG_MODULE_NAME dbnc
#if defined(NDEBUG)
#define LOG_MODULE_LEVEL (0)
#else
#define LOG_MODULE_LEVEL (3)
#endif
#include "log_libs.h"
//<<----------------------

//>>---------------------- Locals
static uint32_t one_number(const uint8_t *buffer, uint32_t size)
{
    uint32_t bits = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        bits += buffer[i];
    }
    return bits;
}

static bool all_number_counter(const uint8_t *buffer, uint32_t size, uint8_t number)
{
    uint32_t bits = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        bits += buffer[i];
    }
    if (number == 0)
        return bits == 0;
    return bits == DEBOUNCE_SIZE;
}

static bool one_counter(const uint8_t *buffer, uint32_t size)
{
    return all_number_counter(buffer, DEBOUNCE_SIZE, 1);
}
//<<----------------------
void dbnc_init(debouncer_t *db, bool state)
{
    db->state_counter = 0;
    db->last_state = state;
    ring_buffer_init(&db->rb, reinterpret_cast<char *>(db->buffer), DEBOUNCE_SIZE);
    while (!dbnc_is_ready(db))
    {
        dbnc_set_current_state(db, state);
    }

    LOG_DEBUG("initial state: %d", dbnc_get_state(db));
}

void dbnc_set_current_state(debouncer_t *db, bool state)
{
    ring_buffer_queue(&db->rb, state);
}

bool dbnc_get_state(const debouncer_t *db)
{
    return one_counter(db->buffer, DEBOUNCE_SIZE);
}

egde_t dbnc_get_edge(debouncer_t *db)
{
    uint32_t ones = one_number(db->buffer, DEBOUNCE_SIZE);
    if (ones == DEBOUNCE_SIZE)
    {
        if (db->last_state != true)
        {
            db->last_state = true;
            return EDGE_RAISING;
        }
        db->state_counter++;
        return EDGE_HIGH_PLATO;
    }
    else if (ones == 0)
    {
        if (db->last_state != false)
        {
            db->last_state = false;
            return EDGE_FALLING;
        }
        db->state_counter++;
        return EDGE_LOW_PLATO;
    }

    db->state_counter = 0;
    return EDGE_BOUNCE;
}

/**
 * @brief Проверить, что кольцевой буфер полный т.е. заполнен адекватными
 * значениями
 *
 * @param db
 * @return true
 * @return false
 */
bool dbnc_is_ready(const debouncer_t *db)
{
    return ring_buffer_is_full(&db->rb);
}

uint16_t dgnc_state_counter(const debouncer_t *db)
{
    return db->state_counter;
}

void dgnc_reset_state_counter(debouncer_t *db)
{
    db->state_counter = 0;
}
