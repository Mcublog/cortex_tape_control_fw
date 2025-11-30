#pragma once

#include "app/ui/Indication.hpp"

class ShiftRegInd final : public Indication
{
  protected:
    void ll_init() const override
    {
    }

    void ll_write(uint8_t d) const override
    {
    }
};
