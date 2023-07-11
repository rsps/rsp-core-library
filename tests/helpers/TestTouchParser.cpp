/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iostream>
#include "TestTouchParser.h"

using namespace rsp::graphics;

TestTouchParser& TestTouchParser::SetEvents(const TestEventItem_t *apTouchEvents, size_t aCount)
{
    mpTouchEvents = apTouchEvents;
    mEventCount = aCount;
    mIndex = 0;
    mpLastEvent = std::make_shared<TouchEvent>();
    return *this;
}

bool TestTouchParser::Poll(GfxEvent &arInput)
{
    if ((mIndex < mEventCount) && (std::chrono::steady_clock::now() >= mpTouchEvents[mIndex].Time)) {
        if (mpTouchEvents[mIndex].Event->IsType<TouchEvent>()) {
            mpLastEvent->Assign(mpTouchEvents[mIndex].Event->CastTo<TouchEvent>());
            arInput = mpLastEvent;
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
    std::cout << "Flushing touch queue." << std::endl;
    mIndex = mEventCount;
}

