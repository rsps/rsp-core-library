/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_INCLUDE_MESSAGING_EVENTBROKER_H
#define RSP_CORE_LIB_INCLUDE_MESSAGING_EVENTBROKER_H

#include <memory>
#include <mutex>
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

    EventBroker& Subscribe(SubscriberInterface &arSubscriber) override;
    EventBroker& Unsubscribe(SubscriberInterface &arSubscriber) override;

protected:
    rsp::logging::LogChannel mLogger;
    std::vector<SubscriberInterface*> mSubscribers{};
    std::vector<EventPtr_t> mQueue{};
    std::mutex mEventBrokerMutex{};
};

} /* namespace rsp::messaging */

#endif /* RSP_CORE_LIB_INCLUDE_MESSAGING_EVENTBROKER_H */
