#include "graphics/messaging/Publisher.h"

namespace rsp::graphics
{
void Publisher::registerBroker(Broker *pBroker)
{
    mBrokerPtr = pBroker;
}

void Publisher::publishToBroker(Topic topic, Event &newEvent)
{
    mBrokerPtr->onPublish(topic, newEvent);
}
} // namespace rsp::graphics
