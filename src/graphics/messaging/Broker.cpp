#include <utility>
#include <algorithm>

#include "graphics/messaging/Broker.h"
#include "graphics/messaging/Publisher.h"
#include "graphics/messaging/Subscriber.h"

namespace rsp::graphics
{
void BrokerBase::addSubscriber(Subscriber &arSubscriber, int aTopic)
{
    mSubscriberMap[aTopic].push_back(&arSubscriber);

// Very cumbersome solution for the above line:
//    // mSubscriberMap.insert(std::pair<std::string, Subscriber *>(topic, ptr));
//    if (auto iter{mSubscriberMap.find(topic)}; iter == mSubscriberMap.end()) {
//        // Topic not found
//        mSubscriberMap.insert(std::pair<Topic, std::vector<Subscriber *>>(topic, std::vector<Subscriber *>{ptr}));
//    } else {
//        // Topic found
//        iter->second.push_back(ptr);
//    }
}

void BrokerBase::removeSubscriber(Subscriber &arSubscriber, int aTopic)
{
    auto topic_it = mSubscriberMap.find(aTopic);
    if (topic_it == mSubscriberMap.end()) {
        return; // No subscribers for this topic, so nothing to remove
    }

    auto sub_it = std::find(topic_it->second.begin(), topic_it->second.end(), &arSubscriber);
    if (sub_it != topic_it->second.end()) {
        topic_it->second.erase(sub_it);
    }
}

void BrokerBase::registerPublisher(Publisher &arPublisher)
{
    arPublisher.registerBroker(*this);
}

void BrokerBase::doPublish(int aTopic, Event &newEvent)
{
    auto it = mSubscriberMap.find(aTopic);
    if (it == mSubscriberMap.end()) {
        return; // No subscribers for this topic
    }

    // foreach sub in multimap for a given key, do->update
    for (auto sub : it->second) {
        sub->updateCallback(newEvent);
    }
}

} // namespace rsp::graphics
