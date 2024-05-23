/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#ifndef RSP_CORE_LIB_INCLUDE_UTILS_THREAD_INTERFACE_H
#define RSP_CORE_LIB_INCLUDE_UTILS_THREAD_INTERFACE_H

#include <exceptions/CoreException.h>
#include <functional>

namespace rsp::utils {

/**
 * \brief Base class for thread exceptions.
 */
class ThreadException : public exceptions::CoreException
{
public:
    ThreadException(const std::string &arName, const char *apMsg)
            : CoreException(std::string("Exception thrown in thread '") + arName + "': " + apMsg)
    {
    }
};

/**
 * \brief Interface description for thread classes.
 */
class ThreadInterface
{
public:
    using ThreadCallback_t = std::function<void(void)>;

    virtual ~ThreadInterface() = default;

    /**
     * \brief Get the name of this thread. If no name have been assigned the thread id is returned.
     * \return string
     */
    [[nodiscard]] virtual const std::string& GetName() const = 0;

    /**
     * \brief Starts the thread
     * \return self
     */
    virtual ThreadInterface &Start() = 0;

    /**
     * \brief Stops the thread and sync result. If the thread have thrown any exceptions, this method will also throw.
     * \return self
     */
    virtual ThreadInterface &Stop() = 0;

    /**
     * \brief Flag the thread for termination. This will stop the execution after the current iteration.
     * \return self
     */
    virtual ThreadInterface &Terminate() = 0;

    /**
     * \brief Check if the thread is terminating
     * \return True if the thread is stopping or stopped.
     */
    [[nodiscard]] virtual bool IsTerminated() const = 0;

    /**
     * \brief Callback for running simple method in own thread
     * \return ThreadCallback_t
     */
    virtual ThreadInterface &SetExecute(ThreadCallback_t aCb) = 0;
};

} // namespace rsp::utils

#endif //RSP_CORE_LIB_INCLUDE_UTILS_THREAD_INTERFACE_H
