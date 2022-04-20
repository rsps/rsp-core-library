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

std::ostream& operator<<(std::ostream &os, const RawTouchEvent &arTouchEvent)
{
    os << "Raw Touch Event: " << arTouchEvent.stime << "." << arTouchEvent.mtime << "\n"
       << "  Type:  " << arTouchEvent.type << "\n"
       << "  Code:  " << arTouchEvent.code << "\n"
       << "  Value: " << arTouchEvent.value;
    return os;
}


InputCreator::InputCreator(const std::string &arPath)
{
    mTouchDevice.Open(arPath, std::ifstream::binary);
}

InputCreator::~InputCreator()
{
    mTouchDevice.Close();
}

bool InputCreator::Poll(Input &arInput)
{
    try {
        arInput.type = readType();
        if (arInput.type != InputType::None) {
            readBody(arInput);
            return true;
        }
    }
    catch(const NoInputData &e) {
    }
    return false;
}

void InputCreator::readRawTouchEvent()
{
    std::size_t len = mTouchDevice.Read(reinterpret_cast<char *>(&mRawTouchEvent), sizeof(RawTouchEvent));
    if (len != sizeof(RawTouchEvent)) {
        throw NoInputData();
    }
}

InputType InputCreator::readType()
{
    readRawTouchEvent();
    if (mRawTouchEvent.code == ABS_MT_SLOT) {
        // New touch detected on multi-touch panel
        readRawTouchEvent();
    }

    if (mRawTouchEvent.type == EV_ABS) {
        if (mRawTouchEvent.code == ABS_MT_TRACKING_ID) {
            if (mRawTouchEvent.value == -1) {
                return InputType::Lift;
            }
            else {
                return InputType::Press;
            }
        } else if (mRawTouchEvent.code == ABS_MT_POSITION_X || mRawTouchEvent.code == ABS_MT_POSITION_Y) {
            return InputType::Drag;
        }
    }

    return InputType::None;
}

void InputCreator::readBody(Input &arInput)
{
    while (mRawTouchEvent.type != EV_SYN) {

        if (mRawTouchEvent.type == EV_ABS) {
            if (mRawTouchEvent.code == ABS_X) {
                arInput.x = mRawTouchEvent.value;
            }
            else if (mRawTouchEvent.code == ABS_Y) {
                arInput.y = mRawTouchEvent.value;
            }
        }
        readRawTouchEvent();
    }
}
} // namespace rsp::graphics
