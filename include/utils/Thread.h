/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_UTILS_THREAD_H
#define RSP_CORE_LIB_UTILS_THREAD_H

#include <exceptions/CoreException.h>
#include <exception>
#include <functional>
#include <string>
#include <string_view>
#include <thread>

namespace rsp::utils {

/**
 * \brief Base class for thread exceptions.
 */
class ThreadException : public exceptions::CoreException
{
public:
    ThreadException(std::string_view aName, const char *aMsg)
        : CoreException(std::string("Exception thrown in thread '" + std::string(aName) + "': " + aMsg))
    {
    }
};

/**
 * \brief Thread object encapsulates a std::thread with an optional name.
 *
 * The object catches an holds an eventual exception terminating the thread.
 *
 */
class Thread
{
public:
    using ThreadCallback_t = std::function<void(void)>;

    Thread() = default;
    explicit Thread(std::string_view aName) : mName(aName) {}
    virtual ~Thread() = default;

    Thread(const Thread &) = delete;
    Thread& operator=(const Thread &) = delete;

    /**
     * \brief Get the name of this thread. If no name have been assigned the thread id is returned.
     * \return string
     */
    std::string GetName();

    /**
     * \brief Set the thread name.
     * \param aName Name of this thread
     * \return self
     */
    Thread& SetName(std::string_view aName) { mName = aName; return *this; }

    /**
     * \brief Starts the thread
     * \return self
     */
    Thread& Start();

    /**
     * \brief Stops the thread and sync result. If the thread have thrown any exceptions, this method will also throw.
     * \return self
     */
    Thread& Stop();

    /**
     * \brief Flag the thread for termination. This will stop the execution after the current iteration.
     * \return self
     */
    Thread& Terminate() { mTerminated = true; return *this; }

    /**
     * \brief Check if the thread is terminating
     * \return True if the thread is stopping or stopped.
     */
    [[nodiscard]] bool IsTerminated() const { return mTerminated; }

    /**
     * \brief Callback for running simple method in own thread
     * \return ThreadCallback_t
     */
    ThreadCallback_t& GetExecute() { return mWhenExecute; }

protected:
    std::string mName{};
    std::thread mThread{};
    ThreadCallback_t mWhenExecute{};
    bool mTerminated = false;
    std::exception_ptr mException = nullptr;

    void run();

    /**
     * \brief Override this for specialized threaded object.
     */
    virtual void execute();
};

} /* namespace rsp::utils */

#endif // RSP_CORE_LIB_UTILS_THREAD_H
