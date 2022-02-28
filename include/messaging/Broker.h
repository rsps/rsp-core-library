/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef BROKER_H
#define BROKER_H

#include <iostream>
#include <map>
#include <vector>

#include "messaging/Event.h"

namespace rsp::messaging
{

class Subscriber;
class Publisher;

class Broker
{
  public:
    virtual ~Broker() {}

    void Subscribe(Subscriber *aPtr, Topic aTopic);
    void Unsubscribe(Subscriber *aPtr, Topic aTopic);
    void RegisterToPublisher(Publisher *aPtr);
    void OnPublish(Topic aTopic, Event &aNewEvent);

    // protected:
    std::map<Topic, std::vector<Subscriber *>> mSubscriberMap;
};
} // namespace rsp::messaging

#endif // BROKER_H