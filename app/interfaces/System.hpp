
#ifndef APP_SYSTEM_INTERFACES_SYSTEM_H
#define APP_SYSTEM_INTERFACES_SYSTEM_H

#include <stdbool.h>
#include <stdint.h>

#include "app/interfaces/Serial.hpp"
#include "app/proto/cobs/Parser.hpp"

class System
{
private:

public:
    virtual void init() = 0;

    virtual void what() = 0;

    virtual Serial *communication_serial() = 0;
    virtual CobsParser *cobs_parser() = 0;

    static void infitite_loop()
    {
        while (1)
        {
        }
    }

    virtual bool prepare_to_sleep()
    {
        return false;
    }

    virtual bool go_to_stanby()
    {
        return false;
    }
};

#endif // APP_SYSTEM_INTERFACES_SYSTEM_H