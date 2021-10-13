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
    Debug      = LOG_DEBUG,  /**< Debug */
};

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

constexpr LogLevel cDefautLogLevel = LogLevel::Info;

/**
 * C++ string to LogLevel operator
 *
 * \param aLevelString
 * \return LogLevel
 */
logging::LogLevel ToLogLevel(std::string aLevelString);

/**
 * C++ LogLevel to string operator
 *
 * \param aLevel
 * \return string
 */
std::string ToString(LogLevel aLevel);

/**
 * \class LogWriterInterface
 * \brief Interface definition for a log writer
 */
class LogWriterInterface {
public:
    virtual ~LogWriterInterface() {}

    /**
     * Write a string to the destination in a thread safe manner.
     *
     * \param arMsg
     * \param aCurrentLevel
     */
    virtual void Write(const std::string &arMsg, LogLevel aCurrentLevel) = 0;

    /**
     * Set the log acceptance level of this writer.
     *
     * \param aLevel
     */
    void SetAcceptLogLevel(LogLevel aLevel) { mAcceptLevel = aLevel; }

protected:
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif /* SRC_LOGGING_LOGWRITER_H_ */
