
#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "app/io/debounce.h"

class Button
{
    using poll_pin_t = bool(&)(void);

  private:
    debouncer_t m_db = {};

    egde_t m_active_edge = egde_t::EDGE_LOW_PLATO;
    uint16_t m_long_press_cnt = 0;
    poll_pin_t &m_poll_pin;

    bool m_short_press = false;
    bool m_pressed = false;
    bool m_long_press = false;

    void press_handling();

    bool press_poll(bool &state)
    {
        bool s = state;
        state = false;
        return s;
    }

  public:
    Button(egde_t active_edge, uint16_t long_press_cnt, poll_pin_t poll_pin)
        : m_active_edge{active_edge}, m_long_press_cnt{long_press_cnt},
          m_poll_pin{poll_pin} {};

    void init();

    bool poll_short_press();
    bool poll_long_press();

    void process();
};

#endif // BUTTON_HPP