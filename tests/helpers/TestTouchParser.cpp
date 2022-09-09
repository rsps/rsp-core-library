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

TestTouchParser& TestTouchParser::SetEvents(const rsp::graphics::TouchEvent *apTouchEvents, std::size_t aCount)
{
    mpTouchEvents = apTouchEvents;
    mEventCount = aCount;
    mIndex = 0;
    return *this;
}

bool TestTouchParser::Poll(rsp::graphics::TouchEvent &arInput)
{
    if ((mIndex < mEventCount) && (std::chrono::steady_clock::now() >= mpTouchEvents[mIndex].mTime)) {
        arInput.Assign(mpTouchEvents[mIndex++]);
        return true;
    }
    return false;
}

void TestTouchParser::Flush()
{
    std::cout << "Flushing touch queue." << std::endl;
    mIndex = mEventCount;
}

