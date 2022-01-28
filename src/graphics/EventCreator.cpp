/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/EventCreator.h"
#include <linux/input.h>

namespace rsp::graphics
{

EventCreator::EventCreator()
{
    // touchDriver.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    touchDriver.Open("/dev/input/event1", std::ifstream::binary);
}

EventCreator::~EventCreator()
{
    touchDriver.Close();
}

bool EventCreator::HasNewEvents()
{
    return touchDriver.WaitForDataReady(50);
}

const Event &EventCreator::GetEvent()
{
    ReadType();
    return event;
}

void EventCreator::ReadType()
{
    touchDriver.Read(reinterpret_cast<char *>(&eventInput), sizeof(eventInput));
    if (eventInput.type == EV_ABS && eventInput.code == ABS_MT_TRACKING_ID && eventInput.value == 0xFFFFFFFF) {
        event.type = EventType::Lift;
    } else if (eventInput.type == EV_ABS && eventInput.code == ABS_MT_TRACKING_ID) {
        event.type = EventType::Press;
    } else if (eventInput.type == EV_ABS &&
               (eventInput.code == ABS_MT_POSITION_X || eventInput.code == ABS_MT_POSITION_Y)) {
        event.type = EventType::Drag;
    }
    ReadBody();
}

void EventCreator::ReadBody()
{
    while (eventInput.type != 0) {
        if (eventInput.type == EV_ABS && eventInput.code == ABS_X) {
            event.x = eventInput.value;
        }
        if (eventInput.type == EV_ABS && eventInput.code == ABS_Y) {
            event.y = eventInput.value;
        }
        touchDriver.Read(reinterpret_cast<char *>(&eventInput), sizeof(eventInput));
    }
}
} // namespace rsp::graphics