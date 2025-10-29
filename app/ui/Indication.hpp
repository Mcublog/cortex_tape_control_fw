#pragma once

#include <cstdint>

#include "app/io/inputs.h"

class Indication
{
  private:
    uint8_t m_state;

protected:
    virtual void ll_init() const = 0;
    virtual void ll_write(uint8_t d) const = 0;

  public:
    void init();

    void set_bit(uint8_t bit_number, bool state);
    void set_inputs(inputs_t i);
};
