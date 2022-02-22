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

#include "graphics/messaging/Event.h"

namespace rsp::graphics
{

class Subscriber;
class Publisher;

class Broker
{
  public:
    virtual ~Broker() {}

    void addSubscriber(Subscriber *ptr, Topic topic);
    void registerToPublisher(Publisher *ptr);
    void onPublish(Topic topic, Event &newEvent);

    // protected:
    std::map<Topic, std::vector<Subscriber *>> mSubscriberMap;
};
} // namespace rsp::graphics

#endif // BROKER_H