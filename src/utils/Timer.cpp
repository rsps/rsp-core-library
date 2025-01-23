/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <vector>
#include <utils/Timer.h>

//#define TLOG(a) DLOG(__FUNCTION__ << ": " << a)
#define TLOG(a)

namespace rsp::utils {

Timer::~Timer()
{
    Enable(false);
}

Timer& Timer::SetTimeout(std::chrono::milliseconds aTimeout)
{
    bool enabled = IsEnabled();
    Enable(false);
    mTimeout = aTimeout;
    Enable(enabled);
    return *this;
}

Timer& Timer::Enable(bool aOn)
{
    if (mEnabled == aOn) {
        return *this;
    }
    mEnabled = aOn;

    if (aOn) {
        TimerQueue::GetInstance().RegisterTimer(this);
    }
    else {
        TimerQueue::GetInstance().UnregisterTimer(this);
    }

    return *this;
}

void Timer::trigger()
{
    if (mEnabled) {
        mEnabled = false;
        mCallback(*this);
    }
}


TimerQueue::TimerQueue()
{
    TLOG("Creating TimerQueue")
    if (!HasInstance()) {
        SetInstance(this);
    }
//    mOffset = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch();
}

TimerQueue::~TimerQueue()
{
    if (&GetInstance() == this) {
        SetInstance(nullptr);
    }
}


void TimerQueue::Poll()
{
    std::vector<Timer*> expired;

    for(;;) {
        if (mQueue.empty()) {
            TLOG("No timers exists.")
            break;
        }
        auto it = mQueue.begin();

        RunTime now;
            TLOG("Timer " << (*it)->GetId()
            << " expires at " << (*it)->mTimeoutAt
            << ". Now: "
            << now)

        if ((*it)->mTimeoutAt <= now) {
            expired.push_back(*it);
            TLOG("Removing timer " << (*it)->GetId())
            mQueue.erase(it);
        }
        else {
            break;
        }
    }

    if (expired.empty()) {
        TLOG("No timers expired.")
        return;
    }
    for(Timer* timer : expired) {
        TLOG("Triggering timer " << timer->GetId())
        timer->trigger();
    }
}

void TimerQueue::RegisterTimer(Timer *apTimer)
{
    if (!apTimer) {
        return;
    }
    apTimer->mTimeoutAt = RunTime(apTimer->GetTimeout());

    for (auto it = mQueue.begin(); it != mQueue.end(); ++it) {
        TLOG("Check timer: " << apTimer->GetId() << ":" << apTimer->mTimeoutAt
            << " < " << (*it)->GetId() << ":" << (*it)->mTimeoutAt)

        if (apTimer->mTimeoutAt < (*it)->mTimeoutAt) {
            TLOG("Inserting timer " << apTimer->GetId() << " before timer " << (*it)->GetId())
            mQueue.insert(it, apTimer);
            return;
        }
    }
    TLOG("Appending timer " << apTimer->GetId())
    mQueue.push_back(apTimer);
}

void TimerQueue::UnregisterTimer(Timer *apTimer)
{
    if (!apTimer) {
        return;
    }
    for (auto it = mQueue.cbegin(); it != mQueue.cend() ; ++it) {
        if (*it == apTimer) {
            TLOG("Removing timer " << (*it)->GetId())
            mQueue.erase(it);
            break;
        }
    }
}

} /* namespace rsp::graphics */
