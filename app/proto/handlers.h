
#ifndef APP_PROTO_HANDLERS_H
#define APP_PROTO_HANDLERS_H

#include <stdint.h>
#include "commands.h"

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t *unk_command_handler(uint32_t id, command_t *outcmd, uint32_t size,
                                 uint32_t *outsize);

    uint8_t *get_version(uint32_t id, command_t *outcmd, uint32_t size,
                         uint32_t *outsize);

    uint8_t *solenoid_pull(uint32_t id, const uint8_t *input, command_t *outcmd,
                           uint32_t size, uint32_t *outsize, command_callback_t clb);

    /**
     * @brief Запрос пользовательского режима механизма по числу.
     *
     * @param mode код mech::UserMode (0–6)
     * @return ack с полем error: 0 — принято, 1 — неверный режим, 2 — отклонено,
     *         3 — обработчик не подключён
     */
    uint8_t *mech_set_mode(uint32_t id, const uint8_t *input, command_t *outcmd,
                           uint32_t size, uint32_t *outsize,
                           mech_mode_callback_t clb);

    /**
     * @brief Высокоуровневая команда транспорта: stop, play, перемотка.
     *
     * @param input структура mech_transport_t с кодом mech_action_t
     * @param clb обработчик, принимающий код mech::UserMode
     * @return ack с полем error: 0 — принято, 1 — неизвестное действие,
     *         2 — отклонено механизмом, 3 — обработчик не подключён
     */
    uint8_t *mech_transport(uint32_t id, const uint8_t *input, command_t *outcmd,
                            uint32_t size, uint32_t *outsize,
                            mech_mode_callback_t clb);

    /**
     * @brief Чтение текущего состояния механизма.
     *
     * @param clb обработчик, заполняющий mech_state_t
     * @return ack с содержимым mech_state_t
     */
    uint8_t *mech_get_state(uint32_t id, command_t *outcmd, uint32_t size,
                            uint32_t *outsize, mech_state_callback_t clb);

#ifdef __cplusplus
}
#endif

#endif // APP_PROTO_HANDLERS_H
