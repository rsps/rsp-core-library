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

#include <logging/LogChannel.h>
#include <string>
#include <string_view>
#include <thread>
#include "ThreadInterface.h"

namespace rsp::utils {

/**
 * \brief Thread object encapsulates a std::thread with an optional name.
 *
 * The object catches an holds an eventual exception terminating the thread.
 *
 */
class Thread : public ThreadInterface, public logging::NamedLogger<Thread>
{
public:
    using ThreadCallback_t = std::function<void(void)>;

    explicit Thread(std::string_view aName);
    ~Thread() override;

    Thread(const Thread &) = delete;
    Thread& operator=(const Thread &) = delete;

    /**
     * \brief Get the name of this thread. If no name have been assigned the thread id is returned.
     * \return string
     */
    [[nodiscard]] const std::string& GetName() const override;

    /**
     * \brief Starts the thread
     * \return self
     */
    ThreadInterface& Start() override;

    /**
     * \brief Stops the thread and sync result. If the thread have thrown any exceptions, this method will also throw.
     * \return self
     */
    ThreadInterface& Stop() override;

    /**
     * \brief Flag the thread for termination. This will stop the execution after the current iteration.
     * \return self
     */
    ThreadInterface& Terminate() override;

    /**
     * \brief Check if the thread is terminating
     * \return True if the thread is stopping or stopped.
     */
    [[nodiscard]] bool IsTerminated() const override;

    /**
     * \brief Callback for running simple method in own thread
     * \return ThreadCallback_t
     */
    ThreadInterface& SetExecute(ThreadCallback_t aCb) override;

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
