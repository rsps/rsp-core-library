/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2023-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <rsp/utils/ThreadGuard.h>

namespace rsp::utils {

ThreadGuard& ThreadGuard::Lock()
{
    std::thread::id self = std::this_thread::get_id();
    std::unique_lock lock{mMutex};
    if (mLockerId != self) {
        mCondition.wait(lock, [this] { return mLockerId == std::thread::id{}; });
        mLockerId = self;
    }

    return *this;
}

bool ThreadGuard::Unlock()
{
    {
        std::lock_guard lock{mMutex};
        if (mLockerId != std::this_thread::get_id()) {
            return false;
        }
        mLockerId = std::thread::id{};
    }

    mCondition.notify_one();
    return true;
}

bool ThreadGuard::IsLocked() const
{
    std::lock_guard lock{mMutex};
    return (mLockerId == std::this_thread::get_id());
}

} // rsp::utils
