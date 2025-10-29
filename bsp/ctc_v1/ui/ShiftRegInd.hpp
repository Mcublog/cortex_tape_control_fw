#pragma once

#include "app/ui/Indication.hpp"

#include "arch/stm32f0/io/shiftregleds.h"

class ShiftRegInd final : public Indication
{
  protected:
    void ll_init() const override
    {
        shftregl_init();
    }

    void ll_write(uint8_t d) const override
    {
        shftregl_write(d);
    }
};
