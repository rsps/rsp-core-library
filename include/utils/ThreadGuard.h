/**
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2023-2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_THREAD_GUARD_H
#define RSP_CORE_LIB_UTILS_THREAD_GUARD_H

#include <thread>
#include <mutex>

namespace rsp::utils {

/**
 * \class ThreadGuard
 *
 * \brief Guard for restricting access to resources to a single class at a time.
 */
class ThreadGuard
{
public:
    ThreadGuard() = default;
    /**
     * \brief Destructor makes sure mutexes are unlocked before destruction.
     */
    ~ThreadGuard();

    /**
     * \brief Check if current thread has locked this
     * \return true if current thread has locked this guard
     */
    [[nodiscard]] bool IsLocked() const;

    /**
     * \brief Lock this guard.
     *
     * If not locked, then lock and return.
     * If already locked by other thread, then wait for unlock, then lock and return.
     * If already locked by this thread, just return.
     *
     * \return self
     */
    ThreadGuard& Lock();

    /**
     * \brief Unlock this guard if current thread is the owner
     * \return True if this guard was unlocked
     */
    bool Unlock();

protected:
    std::thread::id mLockerId{};
    std::mutex mMutex{};
    std::mutex mLock{};

};

} // rsp::utils

#endif // RSP_CORE_LIB_UTILS_THREAD_GUARD_H
