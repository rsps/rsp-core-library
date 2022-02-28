#ifndef BROKER_H
#define BROKER_H

#include <iostream>
#include <map>
#include <vector>

#include "graphics/messaging/Event.h"

namespace rsp::graphics
{
/*
 * TODO: Enumerated Topic should not be declared here. It would severely limit the usage of the component.
 */
enum class Topic {
    Base,
    Email,
    SmS,
    BadTopic
};

class Subscriber;
class Publisher;

class BrokerBase
{
public:
    virtual ~BrokerBase() {}

    void registerPublisher(Publisher &arPublisher); // TODO: Do we need this?

protected:
    std::map<int, std::vector<Subscriber *>> mSubscriberMap;

    void addSubscriber(Subscriber& arSubscriber, int aTopic);
    friend Subscriber;
    void removeSubscriber(Subscriber& arSubscriber, int aTopic);
    void doPublish(int aTopic, Event& arNewEvent);
};

/**
 * \class Broker<T>
 * \brief Broker template for enumerated topic types.
 * \tparam T Enum type used for topic.
 */
template <typename T>
class Broker : public BrokerBase
{
public:
    /**
     * \brief Add a subscriber to the broker
     * \param arSubscriber Reference to the subscriber that is registering
     * \param aTopic Topic to subscribe to
     */
    void AddSubscriber(Subscriber& arSubscriber, T aTopic)
    {
        addSubscriber(arSubscriber, static_cast<int>(aTopic));
    }

    /**
     * \brief Remove a subscriber from this broker
     *
     * Note: This could be superfluous if we always unsubscribe via Subscriber destruction
     *
     * \param arSubscriber Reference to the subscriber leaving
     * \param aTopic Topic to unsubscribe from
     */
    void RemoveSubscriber(Subscriber& arSubscriber, T aTopic)
    {
        RemoveSubscriber(arSubscriber, static_cast<int>(aTopic));
    }

    /**
     * \brief Publish an event through this broker
     * \param aTopic Topic to publish to
     * \param arNewEvent Event to publish
     */
    void Publish(T aTopic, Event &arNewEvent)
    {
        doPublish(static_cast<int>(aTopic), arNewEvent);
    }
};

} // namespace rsp::graphics

#endif // BROKER_H
