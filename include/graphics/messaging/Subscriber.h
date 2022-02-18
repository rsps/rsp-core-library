#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "graphics/messaging/Broker.h"
#include <iostream>

namespace rsp::graphics
{
class Subscriber
{
  public:
    virtual ~Subscriber() {}

    virtual void subscribeToBroker(Broker *ptr, Topic topic);
    virtual void updateCallback(Event &newEvent);

    // protected:
    Topic mTopic;
};
} // namespace rsp::graphics

#endif // SUBSCRIBER_H