#ifndef INPUT_H
#define INPUT_H

#include <iostream>
#include <linux/input.h>

namespace rsp::graphics
{

struct InputLine {
    struct timeval time;
    uint16_t type;
    uint16_t code;
    uint32_t value;
} __attribute__((packed));

enum class InputType {
    Press,
    Drag,
    Lift
};

class Input
{
  public:
    InputType type;
    uint32_t x = 0;
    uint32_t y = 0;

    Input()
    {
    }
    ~Input()
    {
    }
};
} // namespace rsp::graphics
#endif // INPUT_H