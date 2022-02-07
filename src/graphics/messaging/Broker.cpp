#include <utility>

#include "graphics/messaging/Broker.h"
#include "graphics/messaging/Publisher.h"
#include "graphics/messaging/Subscriber.h"

namespace rsp::graphics
{
void Broker::addSubscriber(Subscriber *ptr, Topic topic)
{
    // mSubscriberMap.insert(std::pair<std::string, Subscriber *>(topic, ptr));
    if (auto iter{mSubscriberMap.find(topic)}; iter == mSubscriberMap.end()) {
        // Topic not found
        mSubscriberMap.insert(std::pair<Topic, std::vector<Subscriber *>>(topic, std::vector<Subscriber *>{ptr}));
    } else {
        // Topic found
        iter->second.push_back(ptr);
    }
}

void Broker::registerToPublisher(Publisher *ptr)
{
    ptr->registerBroker(this);
}

void Broker::onPublish(Topic topic, Event &newEvent)
{
    // foreach sub in multimap for a given key, do->update
    for (auto sub : mSubscriberMap[topic]) {
        sub->updateCallback(newEvent);
    }
}
} // namespace rsp::graphics
