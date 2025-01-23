/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "TestTouchParser.h"

using namespace rsp::graphics;

TestTouchParser& TestTouchParser::SetEvents(const TestEventItem_t *apTouchEvents, size_t aCount)
{
    mpTouchEvents = apTouchEvents;
    mEventCount = aCount;
    mIndex = 0;
    return *this;
}

bool TestTouchParser::Poll(GfxEvent &arInput)
{
    auto now = std::chrono::steady_clock::now();
    if ((mIndex < mEventCount) && (now >= mpTouchEvents[mIndex].Time)) {
        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - mpTouchEvents[mIndex].Time).count();
        if (delay > mMaxDelay) {
            mMaxDelay = int64_t(delay);
            mLogger.Debug() << "New maximum event delay: " << delay << "ms";
        }
        if (mpTouchEvents[mIndex].Event->IsType<TouchEvent>()) {
            mLastEvent.Assign(mpTouchEvents[mIndex].Event->CastTo<TouchEvent>());
            arInput = std::make_shared<TouchEvent>(mLastEvent);
        }
        else {
            arInput = mpTouchEvents[mIndex].Event;
        }
        mIndex++;
        return true;
    }
    return false;
}

void TestTouchParser::Flush()
{
    mLogger.Info() << "Flushing touch queue.";
    mIndex = mEventCount;
}

int64_t TestTouchParser::GetMaxDelay() const
{
    return mMaxDelay;
}
