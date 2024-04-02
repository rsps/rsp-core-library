/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2023-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <utils/ThreadGuard.h>

namespace rsp::utils {

ThreadGuard::~ThreadGuard()
{
    std::lock_guard guard(mMutex);
    mLock.try_lock();
    mLock.unlock();
}

ThreadGuard& ThreadGuard::Lock()
{
    while (mLockerId != std::this_thread::get_id())
    {
        {
            std::lock_guard guard(mMutex);

            if (mLockerId == std::thread::id()) { // if empty id
                // Not set
                mLock.lock();
                mLockerId = std::this_thread::get_id();
                break;
            }
        }

        std::this_thread::yield();
//        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return *this;
}

bool ThreadGuard::Unlock()
{
    std::lock_guard guard(mMutex);
    if (mLockerId == std::this_thread::get_id()) {
        mLock.unlock();
        mLockerId = std::thread::id(); // Set to empty id
        return true;
    }

    return false;
}

bool ThreadGuard::IsLocked() const
{
    return (mLockerId == std::this_thread::get_id());
}

} // rsp::utils
