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

namespace rsp::logging {

Logger::Logger(bool aCaptureClog)
    : mpClogBackup(makeCLogStream(aCaptureClog))
{
}

Logger::~Logger()
{
    if (mpClogBackup) {
        OutStreamBuffer *buf = dynamic_cast<OutStreamBuffer*>(std::clog.rdbuf(mpClogBackup.get())); // Restore backup before delete, to avoid segfault.
        mpClogBackup.reset();
        if (buf) {
            // If the old stream_buf was our OutStreamBuffer, then delete it. It could be set elsewhere.
            delete buf;
        }
    }
}

std::shared_ptr<std::streambuf> Logger::makeCLogStream(bool aCaptureLog)
{
    std::streambuf *old = nullptr;
    if (aCaptureLog) {
        old = std::clog.rdbuf(new OutStreamBuffer(this, cDefautLogLevel));
    }
    // Create shared_ptr with "do nothing" deallocator
    return std::shared_ptr<std::streambuf>(old, [](std::streambuf*){});
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

