/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/Logger.h>
#include <utils/Thread.h>
#include <utils/ThreadList.h>

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
    Logger::GetDefault().Info() << "Running thread '" << mName << "'";
    try {
        try {
            while (!mTerminated) {
                execute();
            }
        }
        catch (const std::exception &e) {
            Logger::GetDefault().Error() << "Thread '" << GetName() << "'" << " excepted with: " << e.what();
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

} /* namespace rsp::utils */
