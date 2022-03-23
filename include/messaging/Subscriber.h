/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "Broker.h"
#include <iostream>

namespace rsp::messaging
{

class SubscriberBase
{
public:
    virtual ~SubscriberBase() {}

    /**
     * \brief Abstract handle for receiving events
     * \param arNewEvent A reference to the event to handle
     */
    virtual void HandleEvent(Event &arNewEvent) = 0;
};

template <typename T>
class Subscriber : public SubscriberBase
{
  public:
    Subscriber(Broker<T> &arBroker) : mrBroker(arBroker) {}
    virtual ~Subscriber()
    {
        // Here we MUST unsubscribe from the broker, since our address is no longer valid.
        mrBroker.RemoveSubscriber(*this);
    }

    /**
     * No copy or move is allowed for this subscriber type.
     * The address of the Subscriber object is stored in the Broker, so it needs to be fixed.
     */
    Subscriber(const Subscriber &) = delete;
    Subscriber(const Subscriber &&) = delete;
    Subscriber &operator=(const Subscriber &) = delete;
    Subscriber &operator=(const Subscriber &&) = delete;

    void Subscribe(T aTopic)
    {
        mrBroker.Subscribe(*this, aTopic);
    }

    void Unsubscribe(T aTopic)
    {
        mrBroker.Unsubscribe(*this, aTopic);
    }

  protected:
    Broker<T> &mrBroker;
};
} // namespace rsp::messaging

#endif // SUBSCRIBER_H
