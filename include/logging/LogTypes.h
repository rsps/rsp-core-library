/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_LOG_TYPES_H
#define RSP_CORE_LIB_LOGGING_LOG_TYPES_H

#include <string>

namespace rsp::logging {

/**
 * Enumeration of LogLevel types.
 * The levels are the same as defined in the POSIX syslog standard
 */
enum class LogLevel {
    Emergency  = 0,  /**< Emergency - system is unusable */
    Alert,           /**< Alert - action must be taken immediately */
    Critical,        /**< Critical - critical conditions */
    Error,           /**< Error - error conditions */
    Warning,         /**< Warning - warning conditions */
    Notice,          /**< Notice - normal but significant condition */
    Info,            /**< Info - informational */
    Debug            /**< Debug - debug-level messages */
};

constexpr LogLevel cDefaultLogLevel = LogLevel::Info;

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




#endif // RSP_CORE_LIB_LOGGING_LOG_TYPES_H
