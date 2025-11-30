
#ifndef TARGETS_DESKTOP_IMPL_SYSTEM_EMU_HPP
#define TARGETS_DESKTOP_IMPL_SYSTEM_EMU_HPP

#include "app/interfaces/System.hpp"

class SystemEmu final : public System
{
  private:

  public:
    void init() override;
    void what(void);

    Serial *communication_serial() override;
    CobsParser *cobs_parser() override;
    Indication *indication() override;
};

#endif // TARGETS_DESKTOP_IMPL_SYSTEM_EMU_HPP