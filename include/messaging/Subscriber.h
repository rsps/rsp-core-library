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

#include "messaging/Broker.h"
#include <iostream>

namespace rsp::messaging
{

class Subscriber
{
  public:
    Subscriber() {}
    virtual ~Subscriber()
    {
        // Here we MUST unsubscribe from the broker, since our address is no longer valid.
        if (mpBroker) {
            mpBroker->removeSubscriber(*this, mTopic);
        }
    }

    /**
     * No copy or move is allowed for this subscriber type.
     * The address of the Subscriber object is stored in the Broker, so it needs to be fixed.
     */
    Subscriber(const Subscriber &) = delete;
    Subscriber(const Subscriber &&) = delete;
    Subscriber &operator=(const Subscriber &) = delete;
    Subscriber &operator=(const Subscriber &&) = delete;

    template <typename T>
    void SubscribeToBroker(BrokerBase &arBroker, T aTopic)
    {
        mTopic = static_cast<int>(aTopic);
        mpBroker = &arBroker;
        arBroker.addSubscriber(*this, mTopic);
    }

    /**
     * \brief Abstract handle for recieving events
     * \param arNewEvent A reference to the event to handle
     */
    virtual void HandleEvent(Event &arNewEvent) = 0;

  protected:
    int mTopic = 0;
    BrokerBase *mpBroker = nullptr;
};
} // namespace rsp::messaging

#endif // SUBSCRIBER_H
