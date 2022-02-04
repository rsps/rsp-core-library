/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "graphics/InputCreator.h"
#include <linux/input.h>

namespace rsp::graphics
{

InputCreator::InputCreator()
{
    // touchDriver.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    touchDriver.Open("/dev/input/event1", std::ifstream::binary);
}

InputCreator::~InputCreator()
{
    touchDriver.Close();
}

bool InputCreator::HasNewInputs()
{
    return touchDriver.WaitForDataReady(50);
}

const Input &InputCreator::GetInput()
{
    ReadType();
    return input;
}

void InputCreator::ReadType()
{
    touchDriver.Read(reinterpret_cast<char *>(&inputLine), sizeof(inputLine));
    if (inputLine.type == EV_ABS && inputLine.code == ABS_MT_TRACKING_ID && inputLine.value == 0xFFFFFFFF) {
        input.type = InputType::Lift;
    } else if (inputLine.type == EV_ABS && inputLine.code == ABS_MT_TRACKING_ID) {
        input.type = InputType::Press;
    } else if (inputLine.type == EV_ABS &&
               (inputLine.code == ABS_MT_POSITION_X || inputLine.code == ABS_MT_POSITION_Y)) {
        input.type = InputType::Drag;
    }
    ReadBody();
}

void InputCreator::ReadBody()
{
    while (inputLine.type != 0) {
        if (inputLine.type == EV_ABS && inputLine.code == ABS_X) {
            input.x = inputLine.value;
        }
        if (inputLine.type == EV_ABS && inputLine.code == ABS_Y) {
            input.y = inputLine.value;
        }
        touchDriver.Read(reinterpret_cast<char *>(&inputLine), sizeof(inputLine));
    }
}
} // namespace rsp::graphics