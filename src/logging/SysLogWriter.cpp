/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef SYSLOG

#include <syslog.h>
#include <rsp/logging/SysLogWriter.h>

namespace rsp::logging {

SysLogWriter::SysLogWriter(std::string aIdentifier, const std::string& arAcceptLevel, LogFacility aType)
    : mIdent(std::move(aIdentifier)),
      mAcceptLevel(ToLogLevel(arAcceptLevel))
{
    openlog(mIdent.c_str(), LOG_PID, static_cast<int>(aType));
}

SysLogWriter::SysLogWriter(std::string aIdentifier, LogLevel aAcceptLevel, LogFacility aType)
    : mIdent(std::move(aIdentifier)),
      mAcceptLevel(aAcceptLevel)
{
    openlog(mIdent.c_str(), LOG_PID, static_cast<int>(aType));
}

SysLogWriter::~SysLogWriter()
{
    closelog();
}

void SysLogWriter::Write(std::string_view aMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData&)
{
    if (!aMsg.empty() && (mAcceptLevel >= aCurrentLevel)) {
        if (!arChannel.empty()) {
            syslog(static_cast<int>(aCurrentLevel), "<%s> %s", arChannel.c_str(), aMsg.data());
        }
        else {
            syslog(static_cast<int>(aCurrentLevel), "%s", aMsg.data());
        }
    }
}


} /* namespace logging */

#endif /* SYSLOG */
