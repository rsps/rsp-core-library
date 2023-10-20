/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */


#ifndef RSP_CORE_LIB_MESSAGING_BROKER_INTERFACE_H
#define RSP_CORE_LIB_MESSAGING_BROKER_INTERFACE_H

#include <memory>
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
    ~BrokerInterface() override
    {
        if (&GetInstance() == this) {
            SetInstance(nullptr);
        }
    }

    virtual size_t ProcessEvents() = 0;

    virtual BrokerInterface& Publish(EventPtr_t apEvent) = 0;

    template<class T, std::enable_if_t<std::is_base_of<rsp::messaging::Event, T>::value, bool> = true>
    BrokerInterface& Publish(const T& arEvent)
    {
        return Publish(std::make_shared<T>(arEvent)); // Copy given event
    }

    template<class T, typename ... Args, std::enable_if_t<std::is_base_of<rsp::messaging::Event, T>::value, bool> = true>
    BrokerInterface& Publish(Args... args)
    {
        return Publish(std::make_shared<T>(args...)); // Create given event
    }

    virtual BrokerInterface& Subscribe(SubscriberInterface &arSubscriber) = 0;
    virtual BrokerInterface& Unsubscribe(SubscriberInterface &arSubscriber) = 0;

    BrokerInterface& Subscribe(SubscriberInterface *apSubscriber) { return Subscribe(*apSubscriber); }
    BrokerInterface& Unsubscribe(SubscriberInterface *apSubscriber) { return Unsubscribe(*apSubscriber); }
};

} // namespace rsp::messaging

#endif // RSP_CORE_LIB_MESSAGING_BROKER_INTERFACE_H
