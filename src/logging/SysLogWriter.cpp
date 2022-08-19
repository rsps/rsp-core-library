/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <syslog.h>
#include <logging/SysLogWriter.h>

namespace rsp::logging {

SysLogWriter::SysLogWriter(std::string aIdent, std::string aAcceptLevel, LogType aType)
    : mIdent(aIdent),
      mAcceptLevel(ToLogLevel(aAcceptLevel))
{
    openlog(mIdent.c_str(), LOG_PID, static_cast<int>(aType));
}

SysLogWriter::SysLogWriter(std::string aIdent, LogLevel aAcceptLevel, LogType aType)
    : mIdent(aIdent),
      mAcceptLevel(aAcceptLevel)
{
    openlog(mIdent.c_str(), LOG_PID, static_cast<int>(aType));
}

SysLogWriter::~SysLogWriter()
{
    closelog();
}

void SysLogWriter::Write(const std::string &arMsg, LogLevel aCurrentLevel)
{
    if (arMsg.length() && (mAcceptLevel >= aCurrentLevel)) {
        syslog(static_cast<int>(aCurrentLevel), "%s", arMsg.c_str());
    }
}


} /* namespace logging */

