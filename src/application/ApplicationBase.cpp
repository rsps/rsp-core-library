/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <application/ApplicationBase.h>

namespace rsp::application {

ApplicationBase* ApplicationBase::mpInstance = nullptr;

ApplicationBase::ApplicationBase()
{
    if (mpInstance) {
        THROW_WITH_BACKTRACE(ESingletonViolation);
    }

    mpInstance = this;
}

ApplicationBase::~ApplicationBase()
{
    mpInstance = nullptr;
}


int ApplicationBase::Run()
{
    while(!mTerminated) {
        try {
            execute();
        }
        catch(const std::exception &e) {
            mLogger.Critical() << "Unhandled exception: " << e.what() << std::endl;
            mApplicationResult = cResultUnhandledError;
            mTerminated = true;
        }
    }
    return mApplicationResult;
}

} /* namespace rsp::application */
