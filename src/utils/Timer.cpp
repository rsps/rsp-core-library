/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/Timer.h>

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
        TimerQueue::Get().RegisterTimer(this);
    }
    else {
        TimerQueue::Get().UnregisterTimer(this);
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


void TimerQueue::Poll()
{
    for(;;) {
        auto it = mQueue.begin();

        if ((*it)->mTimeoutAt <= std::chrono::steady_clock::now()) {
            Timer* timer = (*it);
            mQueue.erase(it);
            timer->trigger();
        }
        else {
            break;
        }
    }
}

void TimerQueue::RegisterTimer(Timer *apTimer)
{
    if (!apTimer) {
        return;
    }
    apTimer->mTimeoutAt

    mQueue[index] = apTimer;
}

void TimerQueue::UnregisterTimer(Timer *apTimer)
{
    if (!apTimer) {
        return;
    }
    for (auto it = mQueue.cbegin(); it != mQueue.cend() ; ++it) {
        if (*it == apTimer) {
            mQueue.erase(it);
            break;
        }
    }
}

} /* namespace rsp::graphics */
