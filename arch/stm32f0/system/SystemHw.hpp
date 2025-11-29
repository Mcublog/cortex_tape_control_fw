
#ifndef TARGETS_DESKTOP_IMPL_SYSTEM_EMU_HPP
#define TARGETS_DESKTOP_IMPL_SYSTEM_EMU_HPP

#include "app/interfaces/System.hpp"

class SystemHW final : public System
{
  private:

  public:

    void init() override;
    void what(void) override;

    Serial *communication_serial() override;
    CobsParser *cobs_parser() override;

    bool prepare_to_sleep() override;
    bool go_to_stanby() override;
};

#endif // TARGETS_DESKTOP_IMPL_SYSTEM_EMU_HPP