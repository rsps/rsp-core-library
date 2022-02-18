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
    int32_t  value;
} __attribute__((packed));

enum class InputType {
    None,
    Press,
    Drag,
    Lift
};

class Input
{
  public:
    InputType type = InputType::None;
    int x = 0;
    int y = 0;

//    Input()
//    {
//    }
//    ~Input()
//    {
//    }
};
} // namespace rsp::graphics
#endif // INPUT_H
