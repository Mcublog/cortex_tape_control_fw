#ifndef APP_MECH_BOARD_MECH_IO_HPP
#define APP_MECH_BOARD_MECH_IO_HPP

class MechIo;

/**
 * @brief Платформенная реализация ввода-вывода механизма.
 *
 * Реализуется в BSP конкретной платы.
 */
MechIo &board_mech_io();

#endif // APP_MECH_BOARD_MECH_IO_HPP
