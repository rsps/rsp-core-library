#ifndef EVENT_H
#define EVENT_H

#include <iostream>
#include <linux/input.h>

namespace rsp::graphics
{

struct EventInput {
    struct timeval time;
    uint16_t type;
    uint16_t code;
    uint32_t value;
} __attribute__((packed));

enum class EventType {
    Press,
    Drag,
    Lift
};

class Event
{
  public:
    EventType type;
    uint32_t x = 0;
    uint32_t y = 0;

    Event()
    {
    }
    ~Event()
    {
    }
};
} // namespace rsp::graphics
#endif // EVENT_H