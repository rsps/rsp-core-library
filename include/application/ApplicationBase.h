/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_APPLICATION_APPLICATIONBASE_H_
#define INCLUDE_APPLICATION_APPLICATIONBASE_H_

#include <application/Exceptions.h>
#include <logging/Logger.h>

namespace rsp::application {

class ApplicationBase
{
public:
    ApplicationBase();
    virtual ~ApplicationBase();
    ApplicationBase(const ApplicationBase &other) = delete;
    ApplicationBase(ApplicationBase &&other) = delete;
    ApplicationBase& operator=(const ApplicationBase &other) = delete;
    ApplicationBase& operator=(ApplicationBase &&other) = delete;

    logging::Logger& GetLog() const { return mLogger; }

    template<class T>
    static T& Get() const {
        if (!mpInstance) {
            THROW_WITH_BACKTRACE(ENoInstance);
        }
        return *static_cast<T*>(mpInstance);
    }

    virtual int Run() = 0;

    template<class T>
    T& Terminate(int aResult) {
        if (!mTerminated) {
            mApplicationResult = aResult;
            mTerminated = true;
        }
        return *static_cast<T*>(this);
    }

protected:
    int mApplicationResult = 0;
    bool mTerminated = false;

private:
    static ApplicationBase* mpInstance;
    logging::Logger mLogger;
};

} /* namespace rsp::application */

#endif /* INCLUDE_APPLICATION_APPLICATIONBASE_H_ */
