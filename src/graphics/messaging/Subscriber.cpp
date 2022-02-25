/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#include "graphics/messaging/Subscriber.h"

namespace rsp::graphics
{
void Subscriber::subscribeToBroker(Broker *ptr, Topic topic)
{
    std::cout << "Subscriber " << this << " subscribing to topic: " << (int)topic << std::endl;
    mTopic = topic;
    ptr->addSubscriber(this, topic);
}

void Subscriber::handleEvent(Event &newEvent)
{
    std::cout << "Base Sub got callback" << std::endl;
}
} // namespace rsp::graphics
