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
#include <logging/Logger.h>

using namespace rsp::logging;

namespace rsp::utils {


std::string Thread::GetName()
{
    if (!mName.empty()) {
        return mName;
    }

    std::stringstream ss;
    ss << mThread.get_id();

    return ss.str();
}

Thread& Thread::Start()
{
    mTerminated = false;
    std::thread thread(&Thread::run, this);
    mThread.swap(thread);

    return *this;
}

Thread& Thread::Stop()
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

void Thread::run()
{
    Logger::GetDefault().Info() << "Running thread '" << mName << "'" << std::endl;
    try {
        try {
            while (!mTerminated) {
                execute();
            }
        }
        catch (const std::exception &e) {
            Logger::GetDefault().Error() << "Thread '" << GetName() << "'" << " excepted with: " << e.what() << std::endl;
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
