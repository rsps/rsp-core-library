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
#include <utils/CoreException.h>
#include <linux/input.h>

namespace rsp::graphics
{

class NoInputData : public rsp::utils::CoreException
{
public:
    NoInputData() : rsp::utils::CoreException("Touch input queue empty") {};
};

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
    try {
        readType();
    }
    catch(const NoInputData &e) {
        input.type = InputType::None;
    }
    return input;
}

void InputCreator::readLine()
{
    std::size_t len = touchDriver.Read(reinterpret_cast<char *>(&inputLine), sizeof(InputLine));
    if (len != sizeof(InputLine)) {
        throw NoInputData();
    }
}

void InputCreator::readType()
{
    readLine();
    if (inputLine.code == ABS_MT_SLOT) {
        readLine();
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
    while (inputLine.type != EV_SYN) {
        if (inputLine.type == EV_ABS && inputLine.code == ABS_X) {
            input.x = inputLine.value;
        }
        if (inputLine.type == EV_ABS && inputLine.code == ABS_Y) {
            input.y = inputLine.value;
        }
        readLine();
    }
}
} // namespace rsp::graphics
