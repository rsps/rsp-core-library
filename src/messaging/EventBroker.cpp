/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <messaging/EventBroker.h>

namespace rsp::messaging {

EventBroker::EventBroker()
{
}

size_t EventBroker::ProcessEvents()
{
    size_t result = 0;
    for (auto event : mQueue) {
        for (SubscriberInterface* sub : mSubscribers) {
            sub->ProcessEvent(*event);
        }
        result++;
    }
    mQueue.clear();
    return result;
}

EventBroker& EventBroker::Publish(std::shared_ptr<Event> apEvent)
{
    mQueue.push_back(apEvent);
    return *this;
}

EventBroker& EventBroker::Subscribe(SubscriberInterface *apSubscriber)
{
    auto it = std::find(mSubscribers.begin(), mSubscribers.end(), apSubscriber);
    if (it == mSubscribers.end()) {
        mSubscribers.push_back(apSubscriber);
    }
    return *this;
}

EventBroker& EventBroker::Unsubscribe(SubscriberInterface *apSubscriber)
{
    auto it = std::find(mSubscribers.begin(), mSubscribers.end(), apSubscriber);
    if (it != mSubscribers.end()) {
        mSubscribers.erase(it);
    }
    return *this;
}

} /* namespace rsp::messaging */
