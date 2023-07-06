/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_MESSAGING_EVENTBROKER_H_
#define INCLUDE_MESSAGING_EVENTBROKER_H_

#include "Event.h"

namespace rsp::messaging {

class EventBroker
{
public:
    EventBroker();

    uint32_t Poll(Event& );
};

} /* namespace rsp::messaging */

#endif /* INCLUDE_MESSAGING_EVENTBROKER_H_ */
