#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "graphics/messaging/Broker.h"
#include <iostream>

namespace rsp::graphics
{
class Subscriber
{
public:
    Subscriber() {}
    virtual ~Subscriber() {
        // Here we MUST unsubscribe from the broker, since our address is no longer valid.
        if (mpBroker) {
            mpBroker->removeSubscriber(*this, mTopic);
        }
    }

    /**
     * No copy or move is allowed for this subscriber type.
     * The address of the Subscriber object is stored in the Broker, so it needs to be fixed.
     */
    Subscriber(const Subscriber&) = delete;
    Subscriber(const Subscriber&&) = delete;
    Subscriber& operator=(const Subscriber&) = delete;
    Subscriber& operator=(const Subscriber&&) = delete;

    template<typename T>
    void subscribeToBroker(BrokerBase &arBroker, T aTopic)
    {
        mTopic = static_cast<int>(aTopic);
        mpBroker = &arBroker;
        arBroker.addSubscriber(*this, mTopic);
    }

    virtual void updateCallback(Event &arNewEvent); // TODO: Make this abstract

protected:
    int mTopic = 0;
    BrokerBase *mpBroker = nullptr;
};
} // namespace rsp::graphics

#endif // SUBSCRIBER_H
