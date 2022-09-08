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

TestTouchParser::TestTouchParser(rsp::graphics::TouchEvent *apTouchEvents, std::size_t aCount)
    : TouchParser(),
      mpTouchEvents(apTouchEvents),
      mEventCount(aCount)
{
}

TestTouchParser& TestTouchParser::SetEvents(rsp::graphics::TouchEvent *apTouchEvents, std::size_t aCount)
{
    mpTouchEvents = apTouchEvents;
    mEventCount = aCount;
    return *this;
}

bool TestTouchParser::Poll(rsp::graphics::TouchEvent &arInput)
{
    if (mEventCount) {
        return getNextEvent(arInput);
    }
    return TouchParser::Poll(arInput);
}

void TestTouchParser::Flush()
{
    std::cout << "Flushing touch queue." << std::endl;
}

bool TestTouchParser::getNextEvent(rsp::graphics::TouchEvent &arInput)
{
    if (mIndex < mEventCount) {
        arInput = mpTouchEvents[mIndex++];
        return true;
    }
    return false;
}
