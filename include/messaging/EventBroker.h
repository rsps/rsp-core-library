/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_MESSAGING_EVENTBROKER_H_
#define INCLUDE_MESSAGING_EVENTBROKER_H_

#include <memory>
#include <vector>
#include <logging/LogChannel.h>
#include "Event.h"

namespace rsp::messaging {

class SubscriberInterface
{
public:
    virtual ~SubscriberInterface() {}
    /**
     * \brief Processes events for this subscriber object
     * \param arEvent Reference to the event being processed
     * \return True to stop propagation
     */
    virtual bool ProcessEvent(Event &arEvent) = 0;
};

class BrokerInterface
{
public:
    virtual ~BrokerInterface() {}

    virtual size_t ProcessEvents() = 0;

    virtual BrokerInterface& Publish(EventPtr_t apEvent) = 0;

    virtual BrokerInterface& Subscribe(SubscriberInterface &arSubscriber) = 0;
    virtual BrokerInterface& Unsubscribe(SubscriberInterface &arSubscriber) = 0;

    BrokerInterface& Subscribe(SubscriberInterface *apSubscriber) { return Subscribe(*apSubscriber); }
    BrokerInterface& Unsubscribe(SubscriberInterface *apSubscriber) { return Unsubscribe(*apSubscriber); }
};

class EventBroker : public BrokerInterface
{
public:
    EventBroker();
    virtual ~EventBroker() {}

    size_t ProcessEvents() override;

    EventBroker& Publish(EventPtr_t apEvent) override;

    EventBroker& Subscribe(SubscriberInterface &arSubscriber) override;
    EventBroker& Unsubscribe(SubscriberInterface &arSubscriber) override;

protected:
    rsp::logging::LogChannel mLogger;
    std::vector<SubscriberInterface*> mSubscribers{};
    std::vector<EventPtr_t> mQueue{};
};

} /* namespace rsp::messaging */

#endif /* INCLUDE_MESSAGING_EVENTBROKER_H_ */
