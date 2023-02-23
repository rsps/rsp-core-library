/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/LoggerInterface.h>

namespace rsp::logging {

std::shared_ptr<LoggerInterface> LoggerInterface::mpDefaultInstance = nullptr;


LoggerInterface::Handle_t LoggerInterface::AddLogWriter(std::shared_ptr<LogWriterInterface> aWriter)
{
    mWriters.push_back(aWriter);
    return reinterpret_cast<Handle_t>(aWriter.get());
}

void LoggerInterface::RemoveLogWriter(Handle_t aHandle)
{
    if (aHandle == 0) {
        return;
    }

    auto it = std::find_if(mWriters.begin(), mWriters.end(), [&](std::shared_ptr<LogWriterInterface> const& arWriter) {
        return aHandle == reinterpret_cast<Handle_t>(arWriter.get());
    });
    if (it != mWriters.end()) {
        mWriters.erase(it);
    }
}

bool LoggerInterface::HasWriters() const
{
    return !mWriters.empty();
}

void LoggerInterface::write(const LogStream &arStream, const std::string &arMsg, const std::string &arChannel, const rsp::utils::DynamicData &arContext)
{
    LogLevel current_level = arStream.GetLevel();
    std::lock_guard<std::recursive_mutex> lock(mMutex);

    for (std::shared_ptr<LogWriterInterface> &w : mWriters) {
        w->Write(arMsg, current_level, arChannel, arContext);
    }
}


LoggerInterface& LoggerInterface::GetDefault()
{
    if (!mpDefaultInstance) {
        THROW_WITH_BACKTRACE1(rsp::utils::NotSetException, "Logger instance not set.");
    }

    return *mpDefaultInstance;
}

void LoggerInterface::SetDefault(LoggerInterface* apLogger)
{
    mpDefaultInstance = std::shared_ptr<LoggerInterface>(apLogger, [](LoggerInterface*){});
}

} /* namespace rsp::logging */

