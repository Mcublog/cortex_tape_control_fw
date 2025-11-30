#ifndef USB_SERIAL_H
#define USB_SERIAL_H

#include "app/interfaces/Serial.hpp"

class USBSerial final : public Serial
{
  private:
    /* data */
  public:
    bool init(ios_ctl_t *ctl) override;
    bool helth(){return true;};
    bool write(uint8_t *data, uint32_t size);
};

#endif // USB_SERIAL_H