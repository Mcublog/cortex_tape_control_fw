
#ifndef APP_PROCESS_EXTERNAL_POWER_PROCESS_HPP
#define APP_PROCESS_EXTERNAL_POWER_PROCESS_HPP

#include <cstdbool>
#include <cstdint>

#include "app/proto/commands.h"

namespace Communication
{
bool process();

void callback_attach_solenoid_pull(command_callback_t c);
void callback_attach_mech_mode(mech_mode_callback_t c);
void callback_attach_mech_state(mech_state_callback_t c);

} // namespace Communication

#endif // APP_PROCESS_EXTERNAL_POWER_PROCESS_HPP
