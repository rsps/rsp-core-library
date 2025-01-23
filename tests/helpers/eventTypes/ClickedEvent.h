/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifndef CLICKEDEVENT_H
#define CLICKEDEVENT_H

#include "messaging/Event.h"
#include <string>

namespace rsp::messaging
{
/**
 * \brief Enum type defining topics for ClickedEvents
 */
enum class ClickTopics {
    NullTopic,
    SceneChange
};

class ClickedEvent : public EventBase<ClickedEvent>
{
  public:
    ClickedEvent(std::string aMessage) : mMessage(aMessage){};
    ~ClickedEvent() {}

    std::string mMessage; /*!< Value of the ClickedEvent */
};
} // namespace rsp::messaging

#endif // CLICKEDEVENT_H
