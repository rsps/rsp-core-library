/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#include <logging/LogChannel.h>
#include <exceptions/CoreException.h>

namespace rsp::logging {

LogChannel::LogChannel(std::string_view aChannel)
    : mChannel(aChannel)
{
}

rsp::logging::LogStream LogChannel::Warning()
{
    return LoggerInterface::GetDefault()->Warning().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Notice()
{
    return LoggerInterface::GetDefault()->Notice().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Error()
{
    return LoggerInterface::GetDefault()->Error().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Alert()
{
    return LoggerInterface::GetDefault()->Alert().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Critical()
{
    return LoggerInterface::GetDefault()->Critical().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Emergency()
{
    return LoggerInterface::GetDefault()->Emergency().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Info()
{
    return LoggerInterface::GetDefault()->Info().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Debug()
{
    return LoggerInterface::GetDefault()->Debug().SetChannel(mChannel);
}

LogChannel::LogChannel(const LogChannel &arOther)
    : mChannel(arOther.mChannel)
{
}

LogChannel& LogChannel::operator =(const LogChannel &arOther)
{
    if (this != &arOther) {
        mChannel = arOther.mChannel;
    }
    return *this;
}

LogChannel::LogChannel(LogChannel&& arOther) noexcept
    : mChannel(std::move(arOther.mChannel))
{
}

LogChannel& LogChannel::operator=(LogChannel&& arOther) noexcept
{
    if (this != &arOther) {
        mChannel = std::move(arOther.mChannel);
    }
    return *this;
}

size_t LogChannel::GetWritersCount() const
{
    return LoggerInterface::GetDefault()->GetWritersCount();
}

LoggerInterface::Handle_t LogChannel::addLogWriter(std::shared_ptr<LogWriterInterface> aWriter)
{
    return LoggerInterface::GetDefault()->addLogWriter(aWriter);
}

void LogChannel::write(const LogStream &arStream, const std::string &arMsg, const std::string &arChannel, const utils::DynamicData &arContext)
{
    LoggerInterface::GetDefault()->write(arStream, arMsg, arChannel, arContext);
}

} /* namespace rsp::logging */
