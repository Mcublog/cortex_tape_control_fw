
#ifndef APP_MECH_TYPES_HPP
#define APP_MECH_TYPES_HPP

#include <cstdint>

namespace mech
{

/**
 * @brief Режимы, выбираемые пользователем.
 *
 * Числовые значения передаются по протоколу, менять порядок нельзя.
 */
enum class UserMode : uint8_t
{
    Stop = 0,
    PlayFwd,
    PlayRev,
    RecFwd,
    RecRev,
    FwindFwd,
    FwindRev,
};

/**
 * @brief Режимы транспорта и субрежимы переходов.
 *
 * Порядок соответствует оригинальной машине состояний AVRTapeControl.
 */
enum class Mode : uint8_t
{
    ToInit = 0,
    SubInit,
    SubToStop,
    SubWaitStop,
    Stop,
    SubToActive,
    SubAct,
    SubWaitDir,
    SubHdDirSel,
    SubWaitPinch,
    SubPinchSel,
    SubWaitTakeup,
    SubTuDirSel,
    SubWaitRun,
    PbFwd,
    PbRev,
    RcFwd,
    RcRev,
    FwFwd,
    FwRev,
    FwFwdHdRev,
    FwRevHdRev,
    SubToHalt,
    Halt,
    Max
};

enum class Error : uint8_t
{
    None = 0x00,
    BadDrive = 0x01,
    NoCtrl = 0x02,
    LogicFault = 0x04,
};

enum class Direction : uint8_t
{
    Fwd = 0,
    Rev,
};

struct Features
{
    bool stop_tacho = false;    // Проверять тахометр в STOP при работающем моторе
    bool reverse_enable = true; // Реверс разрешён
    bool auto_reverse = false;  // Автореверс: PB FWD -> PB REV
    bool play_loop = false;     // Цикл: PB REV -> PB FWD, RC REV -> RC FWD
    bool pb_f2rew = false;      // PB FWD -> FW REV по таймауту тахометра
    bool ff_2rew = false;       // FW FWD -> FW REV по таймауту тахометра
};

struct Sensors
{
    bool home = false;
    bool tacho = false;
    bool tape_in = true;
    bool no_rec_fwd = false;
    bool no_rec_rev = false;

    uint8_t to_byte() const
    {
        return (home ? 0x01 : 0x00) | (tacho ? 0x02 : 0x00) |
               (tape_in ? 0x04 : 0x00) | (no_rec_fwd ? 0x08 : 0x00) |
               (no_rec_rev ? 0x10 : 0x00);
    }
};

} // namespace mech

#endif // APP_MECH_TYPES_HPP
