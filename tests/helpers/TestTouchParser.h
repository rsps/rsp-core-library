/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef TESTS_HELPERS_TESTTOUCHPARSER_H_
#define TESTS_HELPERS_TESTTOUCHPARSER_H_

#include <chrono>
#include <graphics/GfxInputEvents.h>

struct TestEventItem_t {
    std::chrono::steady_clock::time_point Time;
    rsp::graphics::GfxEvent Event;

    TestEventItem_t() : Time{}, Event{} {}
    TestEventItem_t(int aOffset, rsp::graphics::GfxEvent &&arEvent)
        : Time(std::chrono::steady_clock::now()),
          Event(std::move(arEvent))
    {
        Time += std::chrono::milliseconds(aOffset);
    }
};

#define MAKE_TOUCH_ITEM(offset, type, point) TestEventItem_t(offset, TouchEvent(offset, type, point))

class TestTouchParser: public rsp::graphics::GfxInputEvents
{
public:
    TestTouchParser() {};
    TestTouchParser(const TestTouchParser&) = delete;
    TestTouchParser& operator=(const TestTouchParser&) = delete;

    TestTouchParser& SetEvents(const TestEventItem_t *apTouchEvents, size_t aCount);

    bool Poll(rsp::graphics::GfxEvent &arInput) override;

    void Flush() override;

protected:
    const TestEventItem_t *mpTouchEvents = nullptr;
    rsp::graphics::TouchEvent mLastEvent{};
    std::size_t mEventCount = 0;
    std::size_t mIndex = 0;
};

#endif /* TESTS_HELPERS_TESTTOUCHPARSER_H_ */
