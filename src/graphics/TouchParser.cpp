/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/TouchParser.h>
#include <utils/CoreException.h>
#include <linux/input.h>

namespace rsp::graphics
{

class NoInputData : public rsp::utils::CoreException
{
public:
    NoInputData() : rsp::utils::CoreException("Touch input queue empty") {};
};

std::ostream& operator<<(std::ostream &os, const RawTouchEvent &arRTE)
{
    os << "Raw Touch Event: " << arRTE.stime << "." << arRTE.mtime << "\n"
       << "  Type:  " << arRTE.type << "\n"
       << "  Code:  " << arRTE.code << "\n"
       << "  Value: " << arRTE.value;
    return os;
}

std::ostream &operator<<(std::ostream &os, const TouchEvent &arTE)
{
    const char* const cTypeNames[] = {
        "None",
        "Press",
        "Drag",
        "Lift"
    };

    os << "Touch Event: " << cTypeNames[static_cast<int>(arTE.mType)] << " " << arTE.mCurrent;

    return os;
}

TouchParser::TouchParser(const std::string &arPath)
{
    mTouchDevice.Open(arPath, std::ifstream::binary);
}

TouchParser::~TouchParser()
{
    mTouchDevice.Close();
}

bool TouchParser::Poll(TouchEvent &arInput)
{
    try {
        arInput.mType = readType();
        if (arInput.mType != TouchEvent::Types::None) {
            readBody(arInput);
            if (arInput.mType == TouchEvent::Types::Press) {
                arInput.mPress = arInput.mCurrent;
            }
            return true;
        }
    }
    catch(const NoInputData &e) {
    }
    return false;
}

void TouchParser::readRawTouchEvent()
{
    std::size_t len = mTouchDevice.Read(reinterpret_cast<void*>(&mRawTouchEvent), sizeof(RawTouchEvent));
    if (len != sizeof(RawTouchEvent)) {
        throw NoInputData();
    }
}

TouchEvent::Types TouchParser::readType()
{
    readRawTouchEvent();
    if (mRawTouchEvent.code == ABS_MT_SLOT) {
        // New touch detected on multi-touch panel
        readRawTouchEvent();
    }

    if (mRawTouchEvent.type == EV_ABS) {
        if (mRawTouchEvent.code == ABS_MT_TRACKING_ID) {
            if (mRawTouchEvent.value == -1) {
                return TouchEvent::Types::Lift;
            }
            else {
                return TouchEvent::Types::Press;
            }
        } else if (mRawTouchEvent.code == ABS_MT_POSITION_X || mRawTouchEvent.code == ABS_MT_POSITION_Y) {
            return TouchEvent::Types::Drag;
        }
    }

    return TouchEvent::Types::None;
}

void TouchParser::readBody(TouchEvent &arInput)
{
    while (mRawTouchEvent.type != EV_SYN) {

        if (mRawTouchEvent.type == EV_ABS) {
            if (mRawTouchEvent.code == ABS_X) {
                arInput.mCurrent.SetX(mRawTouchEvent.value);
            }
            else if (mRawTouchEvent.code == ABS_Y) {
                arInput.mCurrent.SetY(mRawTouchEvent.value);
            }
        }
        readRawTouchEvent();
    }
}

void TouchParser::Flush()
{
    TouchEvent dummy;
    while(Poll(dummy)) {
        continue;
    }
}

} // namespace rsp::graphics
