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
#include <logging/Logger.h>

namespace rsp::logging {

//#define DEBUG(a) DLOG(a)
#define DEBUG(a)

static const std::map<LogLevel, std::string> cLevelToText = {
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
    for (auto m : cLevelToText) {
        if (m.second == aLevelString) {
            return m.first;
        }
    }

    return cDefautLogLevel;
}

std::string ToString(LogLevel aLevel)
{
    return cLevelToText.at(aLevel);
}

std::ostream& operator<<(std::ostream &o, LogLevel aLevel)
{
    o << ToString(aLevel);
    return o;
}

// TODO: Make this into a stream manipulator function
//std::ostream& operator<<(std::ostream &o, LogLevel aLevel)
//{
//    OutStreamBuffer *stream = static_cast<OutStreamBuffer *>(o.rdbuf());
//
//    stream->Lock();
//    DEBUG("Locked by " << std::this_thread::get_id());
//    stream->SetLevel(aLevel);
//
//    return o;
//}


} /* namespace rsp::logging */

