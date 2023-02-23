/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <map>
#include <string>
#include <logging/LogTypes.h>
#include <logging/OutStreamBuffer.h>
#include <magic_enum.hpp>

namespace rsp::logging {

//struct LogNameEntry {
//    LogLevel level;
//    const char *name;
//};
//static LogNameEntry cLogLevelNames[] = {
//    { LogLevel::Emergency, "emergency" },
//    { LogLevel::Alert, "alert" },
//    { LogLevel::Critical, "critical" },
//    { LogLevel::Error, "error" },
//    { LogLevel::Warning, "warning" },
//    { LogLevel::Notice, "notice" },
//    { LogLevel::Info, "info" },
//    { LogLevel::Debug, "debug" }
//};

LogLevel ToLogLevel(std::string aLevelString)
{
    return magic_enum::enum_cast<LogLevel>(aLevelString).value();
}

std::string ToString(LogLevel aLevel)
{
    return std::string(magic_enum::enum_name<LogLevel>(aLevel));
}

std::ostream& operator<<(std::ostream &o, LogLevel aLevel)
{
    o << magic_enum::enum_name<LogLevel>(aLevel);
    return o;
}


} /* namespace rsp::logging */

