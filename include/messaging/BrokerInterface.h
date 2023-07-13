/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef INCLUDE_MESSAGING_BROKERINTERFACE_H_
#define INCLUDE_MESSAGING_BROKERINTERFACE_H_

#include "SubscriberInterface.h"
#include <utils/Singleton.h>

namespace rsp::messaging {

class BrokerInterface : public rsp::utils::Singleton<BrokerInterface>
{
public:
    BrokerInterface()
    {
        if (!HasInstance()) {
            SetInstance(this);
        }
    }
    virtual ~BrokerInterface()
    {
        if (&GetInstance() == this) {
            SetInstance(nullptr);
        }
    }

    virtual size_t ProcessEvents() = 0;

    virtual BrokerInterface& Publish(EventPtr_t apEvent) = 0;

    virtual BrokerInterface& Subscribe(SubscriberInterface &arSubscriber) = 0;
    virtual BrokerInterface& Unsubscribe(SubscriberInterface &arSubscriber) = 0;

    BrokerInterface& Subscribe(SubscriberInterface *apSubscriber) { return Subscribe(*apSubscriber); }
    BrokerInterface& Unsubscribe(SubscriberInterface *apSubscriber) { return Unsubscribe(*apSubscriber); }
};

} // namespace rsp::messaging

#endif /* INCLUDE_MESSAGING_BROKERINTERFACE_H_ */
