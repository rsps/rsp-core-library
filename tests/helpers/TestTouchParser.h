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
#include <memory>
#include <messaging/Event.h>
#include <graphics/GfxInputEvents.h>
#include <logging/LogChannel.h>

struct TestEventItem_t {
    std::chrono::steady_clock::time_point Time{};
    std::shared_ptr<rsp::messaging::Event> Event{};

    TestEventItem_t() {}

    TestEventItem_t(int aOffset, std::shared_ptr<rsp::messaging::Event> &&arEvent)
        : Time(std::chrono::steady_clock::now() + std::chrono::milliseconds(aOffset)),
          Event(std::move(arEvent))
    {
    }
};

template<class T, typename... Args>
static TestEventItem_t MakeEventItem(int aOffset, Args &&... args)
{
    return TestEventItem_t(
        aOffset,
        std::make_shared<T>(std::forward<Args>(args)...)
    );
}

#define MAKE_TOUCH_ITEM(_OFFSET_, _TYPE_, _POINT_) MakeEventItem<TouchEvent>(_OFFSET_, _OFFSET_, _TYPE_, Point(_POINT_))


class TestTouchParser: public rsp::graphics::GfxInputEvents, public rsp::logging::NamedLogger<TestTouchParser>
{
public:
    TestTouchParser() {};
    TestTouchParser(const TestTouchParser&) = delete;
    TestTouchParser& operator=(const TestTouchParser&) = delete;

    TestTouchParser& SetEvents(const TestEventItem_t *apTouchEvents, size_t aCount);

    bool Poll(rsp::graphics::GfxEvent &arEvent) override;

    void Flush() override;

    int64_t GetMaxDelay() const;

protected:
    const TestEventItem_t *mpTouchEvents = nullptr;
    rsp::graphics::TouchEvent mLastEvent{};
    size_t mEventCount = 0;
    size_t mIndex = 0;
    int64_t mMaxDelay = 0;
};

#endif /* TESTS_HELPERS_TESTTOUCHPARSER_H_ */
