/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include "graphics/messaging/Broker.h"
#include <iostream>

namespace rsp::graphics
{
class Subscriber
{
  public:
    virtual ~Subscriber() {}

    virtual void SubscribeToBroker(Broker *aPtr, Topic aTopic);
    virtual void UnsubscribeToBroker(Broker *aPtr, Topic aTopic);
    virtual void HandleEvent(Event &aNewEvent);

    // protected:
    Topic mTopic;
};
} // namespace rsp::graphics

#endif // SUBSCRIBER_H