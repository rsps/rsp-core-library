/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "messaging/Broker.h"
#include <iostream>

namespace rsp::messaging
{
class Publisher
{
  public:
    Publisher()
        : mBrokerPtr(nullptr)
    {
    }
    virtual ~Publisher() {}

    virtual void RegisterBroker(Broker *apBroker);
    virtual void PublishToBroker(Topic aTopic, Event &aNewEvent);

    // protected:
    Broker *mBrokerPtr;
};
} // namespace rsp::messaging

#endif // PUBLISHER_H