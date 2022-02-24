#include "graphics/messaging/Subscriber.h"

namespace rsp::graphics
{
void Subscriber::subscribeToBroker(Broker *ptr, Topic topic)
{
    std::cout << "Subscriber " << this << " subscribing to topic: " << (int)topic << std::endl;
    mTopic = topic;
    ptr->addSubscriber(this, topic);
}

void Subscriber::handleEvent(Event &newEvent)
{
    std::cout << "Base Sub got callback" << std::endl;
}
} // namespace rsp::graphics
