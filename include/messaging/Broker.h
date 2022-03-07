/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef BROKER_H
#define BROKER_H

#include <iostream>
#include <map>
#include <vector>

#include "messaging/Event.h"

namespace rsp::messaging
{

class Subscriber;
class Publisher;

class BrokerBase
{
  public:
    virtual ~BrokerBase() {}

  protected:
    std::map<int, std::vector<Subscriber *>> mSubscriberMap{};

    friend Publisher;
    friend Subscriber;
    void doPublish(int aTopic, Event &arNewEvent);
    void addSubscriber(Subscriber &arSubscriber, int aTopic);
    void removeSubscriber(Subscriber &arSubscriber, int aTopic);
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
    void AddSubscriber(Subscriber &arSubscriber, T aTopic)
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
    void RemoveSubscriber(Subscriber &arSubscriber, T aTopic)
    {
        removeSubscriber(arSubscriber, static_cast<int>(aTopic));
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

} // namespace rsp::messaging

#endif // BROKER_H
