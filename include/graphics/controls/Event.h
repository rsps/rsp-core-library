#pragma once

#include <iostream>
#include <linux/input.h>

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