/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <exceptions/CoreException.h>
#include <iostream>
#include <sstream>
#include <map>
#include <logging/Logger.h>
#include <logging/OutStreamBuffer.h>
#include <thread>

namespace rsp::logging {

Logger::Logger(bool aCaptureClog)
    : mpClogBackup(makeCLogStream(aCaptureClog))
{
}

Logger::~Logger()
{
    if (mpClogBackup) {
        auto *buf = dynamic_cast<OutStreamBuffer*>(std::clog.rdbuf(mpClogBackup.get())); // Restore backup before delete, to avoid segfault.
        mpClogBackup.reset();
        // If the old stream_buf was our OutStreamBuffer, then delete it. It could be set elsewhere.
        delete buf;
    }
}

std::shared_ptr<std::streambuf> Logger::makeCLogStream(bool aCaptureLog)
{
    std::streambuf *old = nullptr;
    if (aCaptureLog) {
        old = std::clog.rdbuf(new OutStreamBuffer(this, cDefaultLogLevel));
    }
    // Create shared_ptr with "do nothing" de-allocator
    return {old, [](std::streambuf*){}};
}

LogStream Logger::Emergency()
{
    return {this, LogLevel::Emergency};
}

LogStream Logger::Alert()
{
    return {this, LogLevel::Alert};
}

LogStream Logger::Critical()
{
    return {this, LogLevel::Critical};
}

LogStream Logger::Error()
{
    return {this, LogLevel::Error};
}

LogStream Logger::Warning()
{
    return {this, LogLevel::Warning};
}

LogStream Logger::Notice()
{
    return {this, LogLevel::Notice};
}

LogStream Logger::Info()
{
    return {this, LogLevel::Info};
}

LogStream Logger::Debug()
{
    return {this, LogLevel::Debug};
}

LoggerInterface::Handle_t Logger::addLogWriter(std::shared_ptr<LogWriterInterface> aWriter)
{
    mWriters.push_back(aWriter);
    return aWriter;
}

size_t Logger::GetWritersCount() const
{
    return mWriters.size();
}

void Logger::write(const LogStream &arStream, const std::string &arMsg, const std::string &arChannel, const rsp::utils::DynamicData &arContext)
{
    LogLevel current_level = arStream.GetLevel();
    std::lock_guard<std::recursive_mutex> lock(mMutex);

    std::erase_if(mWriters, [](auto ptr) { return ptr.expired(); });

    for (const auto& weak_ptr : mWriters) {
        auto writer = weak_ptr.lock();
        if (writer) {
            writer->Write(arMsg, current_level, arChannel, arContext);
        }
    }
}

} /* namespace rsp */
