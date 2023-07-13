/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#ifdef USE_GFX_SW

#include <exceptions/CoreException.h>
#include "TouchParser.h"
#include <linux/input.h>

namespace rsp::graphics::sw {


class NoInputData : public exceptions::CoreException
{
public:
    NoInputData() : CoreException("Touch input queue empty") {};
};

std::ostream& operator<<(std::ostream &os, const RawTouchEvent &arRTE)
{
    os << "Raw Touch Event: " << arRTE.stime << "." << arRTE.mtime << "\n"
       << "  Type:  " << arRTE.type << "\n"
       << "  Code:  " << arRTE.code << "\n"
       << "  Value: " << arRTE.value;
    return os;
}

const char *TouchParser::mpDevicePath = "/dev/input/event1";

TouchParser::TouchParser()
{
    if (mpDevicePath) {
        mTouchDevice.Open(mpDevicePath, std::ifstream::binary);
    }
}

TouchParser::~TouchParser()
{
    mTouchDevice.Close();
}

bool TouchParser::Poll(GfxEvent &arInput)
{
    try {
        mLastEvent.mType = readType();
        if (mLastEvent.mType != TouchTypes::None) {
            readBody(mLastEvent);
            if (mLastEvent.mType == TouchTypes::Press) {
                mLastEvent.mPress = mLastEvent.mCurrent;
                mLastEvent.mPressTime = mLastEvent.mTime;
            }
            arInput = std::make_shared<TouchEvent>(mLastEvent);
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

TouchTypes TouchParser::readType()
{
    if (!mTouchDevice.WaitForDataReady(1)) {
        return TouchTypes::None;
    }

    readRawTouchEvent();
    if (mRawTouchEvent.code == ABS_MT_SLOT) {
        // New touch detected on multi-touch panel
        readRawTouchEvent();
    }

    if (mRawTouchEvent.type == EV_ABS) {
        if (mRawTouchEvent.code == ABS_MT_TRACKING_ID) {
            if (mRawTouchEvent.value == -1) {
                return TouchTypes::Lift;
            }
            else {
                return TouchTypes::Press;
            }
        } else if (mRawTouchEvent.code == ABS_MT_POSITION_X || mRawTouchEvent.code == ABS_MT_POSITION_Y) {
            return TouchTypes::Drag;
        }
    }

    return TouchTypes::None;
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

    std::uint64_t m = (mRawTouchEvent.stime * 1000) + static_cast<uint32_t>(mRawTouchEvent.mtime / 1000);
    arInput.mTime = std::chrono::steady_clock::time_point(std::chrono::milliseconds{m});
}

void TouchParser::Flush()
{
    GfxEvent dummy;
    while(Poll(dummy)) {
        continue;
    }
}

} // namespace rsp::graphics::sw

#endif // USE_GFX_SW
