/**
 * @file mech_io.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Привязка входов-выходов CRP42602Y для платы CTC v1.x.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include "app/interfaces/MechIo.hpp"

#include <cstdint>

#include "app/io/igpio.h"

namespace
{

// Привязка датчика кассеты ещё не проверена на реальном механизме.
// Оставляем выключенной, чтобы автомат считал кассету установленной.
constexpr bool kUseTapeInSwitch = false;
constexpr bool kUseRecInhibitSwitches = false;

constexpr uint8_t kTapeInSw = 1;
constexpr uint8_t kNoRecFwdSw = 2;
constexpr uint8_t kNoRecRevSw = 3;

bool read_switch(uint8_t number)
{
    switch (number)
    {
    case 1:
        return io_gpio_sw1();
    case 2:
        return io_gpio_sw2();
    case 3:
        return io_gpio_sw3();
    case 4:
        return io_gpio_sw4();
    default:
        return false;
    }
}

} // namespace

class BoardMechIo final : public MechIo
{
  public:
    void solenoid(bool on) override
    {
        io_gpio_solenoid(on);
    }

    void capstan(bool on) override
    {
        // На CTC v1.x POWER_ON управляет питанием VCC_MECH/VCC_SOLENOID.
        io_gpio_power(on);
    }

    void mute(bool) override
    {
        // На CTC v1.x нет цепи MUTE.
    }

    void rec_en(bool) override
    {
        // На CTC v1.x нет цепи REC_EN.
    }

    bool home() override
    {
        return io_gpio_brake_sens();
    }

    bool tacho() override
    {
        return io_gpio_tacho();
    }

    bool tape_in() override
    {
        if (!kUseTapeInSwitch)
            return true;

        return read_switch(kTapeInSw);
    }

    bool no_rec_fwd() override
    {
        if (!kUseRecInhibitSwitches)
            return false;

        return read_switch(kNoRecFwdSw);
    }

    bool no_rec_rev() override
    {
        if (!kUseRecInhibitSwitches)
            return false;

        return read_switch(kNoRecRevSw);
    }
};

MechIo &board_mech_io()
{
    static BoardMechIo io;
    return io;
}
