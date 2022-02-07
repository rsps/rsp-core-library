#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "graphics/messaging/Broker.h"
#include <iostream>

namespace rsp::graphics
{
class Publisher
{
  public:
    Publisher()
        : mBrokerPtr(nullptr)
    {
    }
    virtual ~Publisher() {}

    virtual void registerBroker(Broker *pBroker);
    virtual void publishToBroker(Topic topic, Event &newEvent);

    // protected:
    Broker *mBrokerPtr;
};
} // namespace rsp::graphics

#endif // PUBLISHER_H