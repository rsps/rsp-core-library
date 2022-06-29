/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_UTILS_THREAD_H_
#define INCLUDE_UTILS_THREAD_H_

#include <exception>
#include <string>
#include <string_view>
#include <thread>
#include "Function.h"
#include "CoreException.h"

namespace rsp::utils {

/**
 * \brief Base class for thread exceptions.
 */
class ThreadException : public CoreException
{
public:
    ThreadException(std::string_view aName, const char *aMsg)
        : CoreException("")
    {
        mMsg = std::string("Exception thrown in thread '" + std::string(aName) + "': " + aMsg);
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
    using ThreadCallback_t = Function<void(void)>;

    Thread() {}
    Thread(std::string_view aName) : mName(aName) {}
    virtual ~Thread() {}

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
     * \brief Stops the thread. If the thread have thrown any exceptions, this method will also throw.
     * \return self
     */
    Thread& Stop();

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

#endif /* INCLUDE_UTILS_THREAD_H_ */
