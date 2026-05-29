/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_MESSAGING_SUBSCRIBER_INTERFACE_H
#define RSP_CORE_LIB_MESSAGING_SUBSCRIBER_INTERFACE_H

#include "Event.h"

namespace rsp::messaging {

/**
 * \class SubscriberInterface
 * \brief Interface for subscribers to EventBrokers.
 *
 * \see BrokerInterface
 */
class SubscriberInterface
{
public:
    virtual ~SubscriberInterface() = default;
    /**
     * \brief Processes events for this subscriber object
     * \param arEvent Reference to the event being processed
     * \return True to stop propagation
     */
    virtual bool ProcessEvent(Event &arEvent) = 0;
};

} // namespace rsp::messaging

#endif // RSP_CORE_LIB_MESSAGING_SUBSCRIBER_INTERFACE_H
