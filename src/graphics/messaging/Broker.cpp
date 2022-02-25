/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#include <utility>

#include "graphics/messaging/Broker.h"
#include "graphics/messaging/Publisher.h"
#include "graphics/messaging/Subscriber.h"

namespace rsp::graphics
{
void Broker::Subscribe(Subscriber *aPtr, Topic aTopic)
{
    if (auto iter{mSubscriberMap.find(aTopic)}; iter == mSubscriberMap.end()) {
        // Topic not found
        mSubscriberMap.insert(std::pair<Topic, std::vector<Subscriber *>>(aTopic, std::vector<Subscriber *>{aPtr}));
    } else {
        // Topic found
        iter->second.push_back(aPtr);
    }
}
void Broker::Unsubscribe(Subscriber *aPtr, Topic aTopic)
{
    auto mapIter{mSubscriberMap.find(aTopic)};
    for (std::vector<Subscriber *>::iterator vecIter = mapIter->second.begin(); vecIter != mapIter->second.end(); vecIter++) {
        if (aPtr == *vecIter) {
            mapIter->second.erase(vecIter);
        }
    }
}

void Broker::RegisterToPublisher(Publisher *aPtr)
{
    aPtr->RegisterBroker(this);
}

void Broker::OnPublish(Topic aTopic, Event &aNewEvent)
{
    // foreach sub in multimap for a given key, do->update
    for (auto sub : mSubscriberMap[aTopic]) {
        sub->HandleEvent(aNewEvent);
    }
}
} // namespace rsp::graphics
