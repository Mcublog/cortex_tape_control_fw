#ifndef APP_MECH_MECH_HPP
#define APP_MECH_MECH_HPP

#include "app/mech/crp42602y.hpp"

namespace Mech
{

/**
 * @brief Создать и инициализировать машину состояний механизма.
 *
 * Вызывается один раз до запуска тиков 2 мс / 20 мс.
 */
void init(MechIo &io);

bool ready();

Crp42602y &instance();

} // namespace Mech

#endif // APP_MECH_MECH_HPP
