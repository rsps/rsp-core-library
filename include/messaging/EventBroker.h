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
#include <utils/Singleton.h>
#include "BrokerInterface.h"

namespace rsp::messaging {


class EventBroker : public BrokerInterface
{
public:
    EventBroker();

    size_t ProcessEvents() override;

    EventBroker& Publish(EventPtr_t apEvent) override;

    template<class T, std::enable_if_t<std::is_base_of<rsp::messaging::Event, T>::value, bool> = true>
    EventBroker& Publish(const T& arEvent)
    {
        return Publish(std::make_shared<T>(arEvent)); // Copy given event
    }

    template<class T, typename ... Args, std::enable_if_t<std::is_base_of<rsp::messaging::Event, T>::value, bool> = true>
    EventBroker& Publish(Args... args)
    {
        return Publish(std::make_shared<T>(args...)); // Create given event
    }

    EventBroker& Subscribe(SubscriberInterface &arSubscriber) override;
    EventBroker& Unsubscribe(SubscriberInterface &arSubscriber) override;

protected:
    rsp::logging::LogChannel mLogger;
    std::vector<SubscriberInterface*> mSubscribers{};
    std::vector<EventPtr_t> mQueue{};
};

} /* namespace rsp::messaging */

#endif /* INCLUDE_MESSAGING_EVENTBROKER_H_ */
