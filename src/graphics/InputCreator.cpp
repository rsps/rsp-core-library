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
std::ostream &operator<<(std::ostream &os, const InputLine &arInputLine)
{
    os << " Input Seconds: " << arInputLine.stime << "\n"
       << " Input Microsec: " << arInputLine.mtime << "\n"
       << " Input Type: " << arInputLine.type << "\n"
       << " Input Code: " << arInputLine.code << "\n"
       << " Input Value: " << arInputLine.value;
    return os;
}

InputCreator::InputCreator(std::string aPath)
{
    touchDriver.Open(aPath, std::ifstream::binary);
}

InputCreator::~InputCreator()
{
    touchDriver.Close();
}

bool InputCreator::HasNewInputs()
{
    return !touchDriver.IsEOF();
}

const Input &InputCreator::GetInput()
{
    readType();
    return input;
}

void InputCreator::readType()
{
    touchDriver.Read(reinterpret_cast<char *>(&inputLine), sizeof(InputLine));
    if (inputLine.code == 0x2f) {
        touchDriver.Read(reinterpret_cast<char *>(&inputLine), sizeof(InputLine));
    }

    if (inputLine.type == EV_ABS && inputLine.code == ABS_MT_TRACKING_ID && inputLine.value == -1) {
        input.type = InputType::Lift;
    } else if (inputLine.type == EV_ABS && inputLine.code == ABS_MT_TRACKING_ID) {
        input.type = InputType::Press;
    } else if (inputLine.type == EV_ABS &&
               (inputLine.code == ABS_MT_POSITION_X || inputLine.code == ABS_MT_POSITION_Y)) {
        input.type = InputType::Drag;
    }
    readBody();
}

void InputCreator::readBody()
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
