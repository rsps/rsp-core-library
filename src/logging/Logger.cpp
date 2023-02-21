/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iostream>
#include <sstream>
#include <map>
#include <logging/Logger.h>
#include <logging/OutStreamBuffer.h>
#include <thread>
#include <utils/CoreException.h>

//#define DEBUG(a) DLOG(a)
#define DEBUG(a)

namespace rsp::logging {

Logger::Logger(bool aCaptureClog)
    : mpClogBackup((!aCaptureClog ? nullptr : std::clog.rdbuf(new OutStreamBuffer(this, cDefautLogLevel))), [](std::streambuf*){})
{
}

Logger::~Logger()
{
    if (mpClogBackup) {
        std::clog.rdbuf(mpClogBackup.get()); // Restore backup before delete, to avoid segfault.
        mpClogBackup.reset();
    }
}

LogStream Logger::Emergency()
{
    return LogStream(this, LogLevel::Emergency, mChannel, mContext);
}

LogStream Logger::Alert()
{
    return LogStream(this, LogLevel::Alert, mChannel, mContext);
}

LogStream Logger::Critical()
{
    return LogStream(this, LogLevel::Critical, mChannel, mContext);
}

LogStream Logger::Error()
{
    return LogStream(this, LogLevel::Error, mChannel, mContext);
}

LogStream Logger::Warning()
{
    return LogStream(this, LogLevel::Warning, mChannel, mContext);
}

LogStream Logger::Notice()
{
    return LogStream(this, LogLevel::Notice, mChannel, mContext);
}

LogStream Logger::Info()
{
    return LogStream(this, LogLevel::Info, mChannel, mContext);
}

LogStream Logger::Debug()
{
    return LogStream(this, LogLevel::Debug, mChannel, mContext);
}

} /* namespace rsp */

