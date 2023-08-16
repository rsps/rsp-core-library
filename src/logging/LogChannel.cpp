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

namespace rsp::logging {

LogChannel::LogChannel(std::string_view aChannel)
    : mrLogger(LoggerInterface::GetDefault()),
      mChannel(aChannel)
{
}

rsp::logging::LogStream LogChannel::Warning()
{
    return mrLogger.Warning().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Notice()
{
    return mrLogger.Notice().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Error()
{
    return mrLogger.Error().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Alert()
{
    return mrLogger.Alert().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Critical()
{
    return mrLogger.Critical().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Emergency()
{
    return mrLogger.Emergency().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Info()
{
    return mrLogger.Info().SetChannel(mChannel);
}

rsp::logging::LogStream LogChannel::Debug()
{
    return mrLogger.Debug().SetChannel(mChannel);
}

LogChannel::LogChannel(const LogChannel &arOther)
    : mrLogger(LoggerInterface::GetDefault()),
      mChannel(arOther.mChannel)
{
}

LogChannel& LogChannel::operator =(const LogChannel &arOther)
{
    if (this != &arOther) {
        mChannel = arOther.mChannel;
    }
    return *this;
}

} /* namespace rsp::logging */
