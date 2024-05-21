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
#include <logging/Logger.h>
#ifdef USE_GFX
    #include <graphics/Control.h>
#endif

using namespace std::ranges;
using namespace rsp::logging;

namespace rsp::messaging {

EventBroker::EventBroker()
    : mLogger("EventBroker")
{
}

size_t EventBroker::ProcessEvents()
{
    size_t result = 0;
    std::vector<EventPtr_t> q;

    {
        std::lock_guard<std::mutex> guard(mEventBrokerMutex);
        q = mQueue;
        mQueue.clear();
    }
    for (auto &event : q) {
        for (SubscriberInterface* &sub : mSubscribers) {
#ifdef USE_GFX
            auto *ctrl = dynamic_cast<rsp::graphics::Control*>(sub);
            mLogger.Debug() << "Propagating " << *event << " to " << (ctrl ? ctrl->GetName() : "Unknown");
#endif
            if (sub->ProcessEvent(*event)) {
                break;
            }
        }
        result++;
    }
    return result;
}

EventBroker& EventBroker::Publish(EventPtr_t apEvent)
{
    mLogger.Debug() << "Publishing " << *apEvent;
    std::lock_guard<std::mutex> guard(mEventBrokerMutex);
    mQueue.push_back(apEvent);
    return *this;
}

EventBroker& EventBroker::Subscribe(SubscriberInterface &arSubscriber)
{
    auto it = std::find(mSubscribers.begin(), mSubscribers.end(), &arSubscriber);
    if (it == mSubscribers.end()) {
        mSubscribers.push_back(&arSubscriber);
    }
    return *this;
}

EventBroker& EventBroker::Unsubscribe(SubscriberInterface &arSubscriber)
{
    auto it = std::find(mSubscribers.begin(), mSubscribers.end(), &arSubscriber);
    if (it != mSubscribers.end()) {
        mSubscribers.erase(it);
    }
    return *this;
}

} /* namespace rsp::messaging */
