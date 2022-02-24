#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "graphics/messaging/Broker.h"
#include <iostream>

namespace rsp::graphics
{

class Publisher
{
public:
    Publisher() : mpBroker(nullptr) {}

    /**
     * \brief Create a publisher for a given broker.
     * \param arBroker Reference to Broker to publish through.
     */
    Publisher(BrokerBase &arBroker) : mpBroker(&arBroker) {}

    Publisher(const Publisher&) = default;
    Publisher& operator=(const Publisher&) = default;

    /**
     * \brief Register a broker with this publisher
     * \param arBroker Reference to Broker
     * \deprecated
     */
    void registerBroker(BrokerBase &arBroker);

    /**
     * \brief Publish the given enumerated topic through the registered broker.
     * \tparam T Enum type for topics
     * \param aTopic Topic to publish to
     * \param arNewEvent Event to publish
     */
    template<typename T>
    void publishToBroker(T aTopic, Event &arNewEvent)
    {
        mpBroker->onPublish(static_cast<int>(aTopic), arNewEvent);
    }

protected:
    /*
     * TODO:
     * By removing registerBroker and only using new constructor, the following
     * can be changed to:
     * BrokerBase &mrBroker;
     */
    BrokerBase *mpBroker;
};
} // namespace rsp::graphics

#endif // PUBLISHER_H
