/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_TIMER_H_
#define INCLUDE_UTILS_TIMER_H_

#include <chrono>
#include <list>
#include "Function.h"
#include "Singleton.h"

namespace rsp::utils {

class TimerQueue;

/**
 * \class Timer
 * \brief Helper class to hold callback to be triggered after a given time.
 */
class Timer
{
public:
    using TimerCallback_t = rsp::utils::Function<void(Timer&)>;

    Timer() {}
    virtual ~Timer();

    Timer& SetId(int aId) { mId = aId; return *this; }
    int GetId() const { return mId; }

    Timer& SetTimeout(std::chrono::milliseconds aTimeout);
    std::chrono::milliseconds GetTimeout() const { return mTimeout; }

    Timer& Enable(bool aOn = true);
    bool IsEnabled() const { return mEnabled; }

    TimerCallback_t& Callback() { return mCallback; }

protected:
    int mId = 0;
    bool mEnabled = false;
    std::chrono::milliseconds mTimeout{};
    TimerCallback_t mCallback{};

    friend TimerQueue;
    std::chrono::time_point<std::chrono::steady_clock> mTimeoutAt{};
    virtual void trigger();
};

/**
 * \class TimerQueue
 * \brief Singleton class with queue for Timer objects.
 */
class TimerQueue : public rsp::utils::Singleton<TimerQueue>
{
public:
    TimerQueue();

    /**
     * \brief Poll the timer queue to trigger expired timers
     */
    void Poll();

    /**
     * \brief Register a Timer object in the callback queue.
     *
     * \param apTimer Pointer to *Timer object
     */
    void RegisterTimer(Timer *apTimer);
    void UnregisterTimer(Timer *apTimer);
protected:
    std::list<Timer*> mQueue{};
    std::chrono::milliseconds mOffset{};

    std::uint64_t Count(std::chrono::steady_clock::time_point aTp);
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_UTILS_TIMER_H_ */
