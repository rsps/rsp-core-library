/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_LOGGING_LOGTYPES_H_
#define INCLUDE_LOGGING_LOGTYPES_H_

#include <string>
#include <syslog.h>

namespace rsp::logging {

/**
 * Enumeration of LogLevel types.
 * The levels are the same as defined in the POSIX syslog standard
 */
enum class LogLevel {
    Emergency  = LOG_EMERG,  /**< Emergency */
    Alert      = LOG_ALERT,  /**< Alert */
    Critical   = LOG_CRIT,   /**< Critical */
    Error      = LOG_ERR,    /**< Error */
    Warning    = LOG_WARNING,/**< Warning */
    Notice     = LOG_NOTICE, /**< Notice */
    Info       = LOG_INFO,   /**< Info */
    Debug      = LOG_DEBUG   /**< Debug */
};

constexpr LogLevel cDefaultLogLevel = LogLevel::Info;

/**
 * The syslog program types.
 */
enum class LogType
{
    Auth   = LOG_AUTH,  /**< Auth */
    Cron   = LOG_CRON,  /**< Cron */
    Daemon = LOG_DAEMON,/**< Daemon */
    Local0 = LOG_LOCAL0,/**< Local0 */
    Local1 = LOG_LOCAL1,/**< Local1 */
    Local2 = LOG_LOCAL2,/**< Local2 */
    Local3 = LOG_LOCAL3,/**< Local3 */
    Local4 = LOG_LOCAL4,/**< Local4 */
    Local5 = LOG_LOCAL5,/**< Local5 */
    Local6 = LOG_LOCAL6,/**< Local6 */
    Local7 = LOG_LOCAL7,/**< Local7 */
    Print  = LOG_LPR,   /**< Print */
    Mail   = LOG_MAIL,  /**< Mail */
    News   = LOG_NEWS,  /**< News */
    User   = LOG_USER,  /**< User */
    Uucp   = LOG_UUCP,  /**< Uucp */
};

/**
 * C++ string to LogLevel operator
 *
 * \param aLevelString
 * \return LogLevel
 */
logging::LogLevel ToLogLevel(const std::string& arLevelString);

/**
 * C++ LogLevel to string operator
 *
 * \param aLevel
 * \return string
 */
std::string ToString(LogLevel aLevel);

/**
 * \fn std::ostream operator <<&(std::ostream, const LogLevel)
 * \brief Output stream operator for LogLevel
 *
 * \param o ostream to write to
 * \param aLevel
 * \return ostream
 */
std::ostream& operator<<(std::ostream &o, LogLevel aLevel);


} /* namespace rsp::logging */




#endif /* INCLUDE_LOGGING_LOGTYPES_H_ */
