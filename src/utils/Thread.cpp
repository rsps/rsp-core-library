/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <utils/Thread.h>
#include <utils/ThreadList.h>
#ifdef ESP_PLATFORM
    #include <esp_pthread.h>
#endif

using namespace rsp::logging;

namespace rsp::utils {

Thread::Thread(std::string_view aName)
      : mName(aName)
{
    if (mName.empty()) {
        THROW_WITH_BACKTRACE2(ThreadException, "<empty>>", "A Thread name must not be empty.");
    }

    ThreadList::GetInstance().AddThread(*this);
}

Thread::~Thread()
{
    ThreadList::GetInstance().RemoveThread(*this);
}

const std::string& Thread::GetName() const
{
    return mName;
}

ThreadInterface& Thread::Start()
{
    mTerminated = false;
    std::thread thread(&Thread::run, this);
    mThread.swap(thread);

    return *this;
}

ThreadInterface& Thread::Stop()
{
    mTerminated = true;

    if (mThread.joinable()) {
        mThread.join();
    }

    if (mException) {
        std::rethrow_exception(mException);
    }

    return *this;
}

ThreadInterface& Thread::Terminate()
{
    mTerminated = true;
    return *this;
}

bool Thread::IsTerminated() const
{
    return mTerminated;
}

ThreadInterface& Thread::SetExecute(Thread::ThreadCallback_t aCb)
{
    mWhenExecute = aCb;
    return *this;
}

void Thread::run()
{
    mLogger.Debug() << "Running thread '" << mName << "'";
    try {
        try {
            while (!mTerminated) {
                execute();
            }
        }
        catch (const std::exception &e) {
            mLogger.Error() << "Thread '" << GetName() << "'" << " excepted with: " << e.what();
            throw ThreadException(GetName(), e.what());
        }
    }
    catch(...) {
        mException = std::current_exception();
    }
}

void Thread::execute()
{
    if (mWhenExecute) {
        mWhenExecute();
    }
    else {
        mTerminated = true;
    }
}

#define UNUSED(macro_arg_parameter) {(void)macro_arg_parameter;}

Thread& Thread::SetAttributes(size_t aStackSize, size_t aPriority, int aCoreId)
{
#ifdef ESP_PLATFORM
    auto cfg = esp_pthread_get_default_config();
    cfg.thread_name = GetName().c_str();
    cfg.pin_to_core = aCoreId;
    cfg.stack_size = aStackSize;
    cfg.prio = aPriority;
    esp_pthread_set_cfg(&cfg);
#else
    UNUSED(aStackSize)
    UNUSED(aPriority)
    UNUSED(aCoreId)
#endif /* ESP_PLATFORM */
    return *this;
}
} /* namespace rsp::utils */
