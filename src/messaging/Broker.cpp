/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#include <algorithm>
#include <utility>

#include <messaging/Broker.h>
#include <messaging/Publisher.h>
#include <messaging/Subscriber.h>

namespace rsp::messaging
{
void BrokerBase::subscribe(SubscriberBase &arSubscriber, int aTopic)
{
    mSubscriberMap[aTopic].push_back(&arSubscriber);
}

void BrokerBase::unsubscribe(SubscriberBase &arSubscriber, int aTopic)
{
    auto topic_it = mSubscriberMap.find(aTopic);
    if (topic_it == mSubscriberMap.end()) {
        return; // No subscribers for this topic, so nothing to remove
    }

    auto sub_it = std::find(topic_it->second.begin(), topic_it->second.end(), &arSubscriber);
    if (sub_it != topic_it->second.end()) {
        topic_it->second.erase(sub_it);
    }
}

void BrokerBase::removeSubscriber(SubscriberBase &arSubscriber)
{
    for (auto &pair : mSubscriberMap) {
        auto sub_it = std::find(pair.second.begin(), pair.second.end(), &arSubscriber);
        if (sub_it != pair.second.end()) {
            pair.second.erase(sub_it);
        }
    }
}

void BrokerBase::doPublish(int aTopic, Event &arNewEvent)
{
    auto it = mSubscriberMap.find(aTopic);
    if (it == mSubscriberMap.end()) {
        return; // No subscribers for this topic
    }

    // foreach sub in multimap for a given key, do->update
    for (SubscriberBase* sub : it->second) {
        sub->HandleEvent(arNewEvent);
    }
}

} // namespace rsp::messaging
