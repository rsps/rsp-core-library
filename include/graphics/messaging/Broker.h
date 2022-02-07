#ifndef BROKER_H
#define BROKER_H

#include <iostream>
#include <map>
#include <vector>

#include "graphics/messaging/Event.h"

namespace rsp::graphics
{
enum class Topic {
    Base,
    Email,
    SmS,
    BadTopic
};

class Subscriber;
class Publisher;

class Broker
{
  public:
    virtual ~Broker() {}

    void addSubscriber(Subscriber *ptr, Topic topic);
    void registerToPublisher(Publisher *ptr);
    void onPublish(Topic topic, Event &newEvent);

    // protected:
    std::map<Topic, std::vector<Subscriber *>> mSubscriberMap;
};
} // namespace rsp::graphics

#endif // BROKER_H