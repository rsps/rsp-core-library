/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_LOGGING_LOGWRITER_H_
#define SRC_LOGGING_LOGWRITER_H_

#include <syslog.h>
#include <string>

namespace rsp::logging {

enum class LogLevel {
    Emergency  = LOG_EMERG,
    Alert      = LOG_ALERT,
    Critical   = LOG_CRIT,
    Error      = LOG_ERR,
    Warning    = LOG_WARNING,
    Notice     = LOG_NOTICE,
    Info       = LOG_INFO,
    Debug      = LOG_DEBUG,
};

enum class LogType
{
    Auth   = LOG_AUTH,
    Cron   = LOG_CRON,
    Daemon = LOG_DAEMON,
    Local0 = LOG_LOCAL0,
    Local1 = LOG_LOCAL1,
    Local2 = LOG_LOCAL2,
    Local3 = LOG_LOCAL3,
    Local4 = LOG_LOCAL4,
    Local5 = LOG_LOCAL5,
    Local6 = LOG_LOCAL6,
    Local7 = LOG_LOCAL7,
    Print  = LOG_LPR,
    Mail   = LOG_MAIL,
    News   = LOG_NEWS,
    User   = LOG_USER,
    Uucp   = LOG_UUCP,
};

constexpr LogLevel cDefautLogLevel = LogLevel::Info;

logging::LogLevel ToLogLevel(std::string aLevelString);
std::string ToString(LogLevel aLevel);

class LogWriterInterface {
public:
    virtual ~LogWriterInterface() {}

    virtual void Write(const std::string &arMsg, LogLevel aCurrentLevel) = 0;

    void SetAcceptLogLevel(LogLevel aLevel) { mAcceptLevel = aLevel; }

protected:
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif /* SRC_LOGGING_LOGWRITER_H_ */
