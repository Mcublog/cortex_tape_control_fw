/**
 * @file mech_io.cpp
 * @author Viacheslav (viacheslav@mcublog.ru)
 * @brief Заглушка входов-выходов CRP42602Y для эмулятора на ПК.
 * @version 0.1
 * @date 2026-10-24
 *
 * @copyright Viacheslav mcublog (c) 2025
 */
#include "app/interfaces/MechIo.hpp"

class EmuMechIo final : public MechIo
{
  public:
    void solenoid(bool) override
    {
    }

    void capstan(bool) override
    {
    }

    void mute(bool) override
    {
    }

    void rec_en(bool) override
    {
    }

    bool home() override
    {
        return true;
    }

    bool tacho() override
    {
        return false;
    }

    bool tape_in() override
    {
        return true;
    }

    bool no_rec_fwd() override
    {
        return false;
    }

    bool no_rec_rev() override
    {
        return false;
    }
};

MechIo &board_mech_io()
{
    static EmuMechIo io;
    return io;
}
