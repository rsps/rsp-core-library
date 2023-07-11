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
#include "Event.h"

namespace rsp::messaging {

class SubscriberInterface
{
public:
    virtual ~SubscriberInterface() {}
    /**
     * \brief Processes events for this subscriber object
     * \param arEvent Reference to the event being processed
     * \return True if handled
     */
    virtual bool ProcessEvent(Event &arEvent) = 0;
};

class BrokerInterface
{
public:
    virtual ~BrokerInterface() {}

    virtual size_t ProcessEvents() = 0;

    virtual BrokerInterface& Publish(std::shared_ptr<Event> apEvent) = 0;

    virtual BrokerInterface& Subscribe(SubscriberInterface *apSubscriber) = 0;
    virtual BrokerInterface& Unsubscribe(SubscriberInterface *apSubscriber) = 0;

};

class EventBroker : public BrokerInterface
{
public:
    EventBroker();
    virtual ~EventBroker() {}

    size_t ProcessEvents() override;

    EventBroker& Publish(std::shared_ptr<Event> apEvent) override;

    EventBroker& Subscribe(SubscriberInterface *apSubscriber) override;
    EventBroker& Unsubscribe(SubscriberInterface *apSubscriber) override;

protected:
    std::vector<SubscriberInterface*> mSubscribers{};
    std::vector<std::shared_ptr<Event>> mQueue{};
};

} /* namespace rsp::messaging */

#endif /* INCLUDE_MESSAGING_EVENTBROKER_H_ */
