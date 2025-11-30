
#ifndef APP_PROTO_PARSER_H
#define APP_PROTO_PARSER_H

#include <cstdbool>
#include <cstdint>

#include "app/interfaces/Serial.hpp"
#include "app/io/serial/types.h"

class CobsParser
{
  private:
    bool m_msg_ready = false;
    ios_ctl_t m_ctl = {};
    uint8_t m_pos = 0;
    Serial *m_sdev = nullptr;

    static constexpr char kBinaryEndChar = '\0';
    bool write(const ios_message_t *msg);

  public:
    bool init(Serial *dev);

    bool is_message_received(void) const;

    ios_message_t *read_message(void);

    uint8_t *get_output_buffer(uint32_t *limit);
    bool write_message(uint8_t *message, uint32_t size);
    bool write_message(uint32_t channel, uint32_t version, uint8_t *message,
                       uint32_t size);

    uint32_t irq_handler(ios_chunk_t *chunk);
};

#endif // APP_PROTO_PARSER_H