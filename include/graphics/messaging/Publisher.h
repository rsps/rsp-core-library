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

#include "graphics/messaging/Broker.h"
#include <iostream>

namespace rsp::graphics
{
class Publisher
{
  public:
    Publisher()
        : mBrokerPtr(nullptr)
    {
    }
    virtual ~Publisher() {}

    virtual void registerBroker(Broker *pBroker);
    virtual void publishToBroker(Topic topic, Event &newEvent);

    // protected:
    Broker *mBrokerPtr;
};
} // namespace rsp::graphics

#endif // PUBLISHER_H