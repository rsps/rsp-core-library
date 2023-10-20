/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_TIMER_H
#define RSP_CORE_LIB_UTILS_TIMER_H

#include <functional>
#include <list>
#include "Singleton.h"
#include "RunTime.h"

namespace rsp::utils {

class TimerQueue;

/**
 * \class Timer
 * \brief Helper class to hold callback to be triggered after a given time.
 */
class Timer
{
public:
    using TimerCallback_t = std::function<void(Timer&)>;

    Timer() {}
    Timer(int aId, std::chrono::milliseconds aTimeout) : mId(aId), mTimeout(aTimeout) {}
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
    RunTime mTimeoutAt{};
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
    ~TimerQueue();

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
};

} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_UTILS_TIMER_H
