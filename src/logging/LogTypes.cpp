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
#include <utils/EnumUtils.h>

namespace rsp::logging {

struct LogNameEntry {
    LogLevel level;
    const char *name;
};
static LogNameEntry cLogLevelNames[] = {
    { LogLevel::Emergency, "emergency" },
    { LogLevel::Alert, "alert" },
    { LogLevel::Critical, "critical" },
    { LogLevel::Error, "error" },
    { LogLevel::Warning, "warning" },
    { LogLevel::Notice, "notice" },
    { LogLevel::Info, "info" },
    { LogLevel::Debug, "debug" }
};

LogLevel ToLogLevel(std::string aLevelString)
{
    rsp::utils::assert_enum_list<sizeof(cLogLevelNames), LogLevel, LogNameEntry>();

    for (auto m : cLogLevelNames) {
        if (m.name == aLevelString) {
            return m.level;
        }
    }

    return cDefautLogLevel;
}

std::string ToString(LogLevel aLevel)
{
    rsp::utils::assert_enum_list<sizeof(cLogLevelNames), LogLevel, LogNameEntry>();

    return cLogLevelNames[static_cast<int>(aLevel)].name;
}

std::ostream& operator<<(std::ostream &o, LogLevel aLevel)
{
    o << ToString(aLevel);
    return o;
}


} /* namespace rsp::logging */

