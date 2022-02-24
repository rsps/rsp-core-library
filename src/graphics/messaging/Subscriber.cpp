#include "graphics/messaging/Subscriber.h"

namespace rsp::graphics
{

void Subscriber::updateCallback(Event &newEvent)
{
    std::cout << "Base Sub got callback" << std::endl;
}
} // namespace rsp::graphics
