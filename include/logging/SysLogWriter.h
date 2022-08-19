/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_LOGGING_SYSLOGWRITER_H_
#define SRC_LOGGING_SYSLOGWRITER_H_

#include "LogWriter.h"

namespace rsp::logging {

/**
 * \class SysLogWriter
 * \brief A log writer that sends output to a syslog server.
 */
class SysLogWriter: public LogWriterInterface {
public:
    SysLogWriter(std::string aIdent, std::string aAcceptLevel, LogType aType);
    SysLogWriter(std::string aIdent, LogLevel aAcceptLevel, LogType aType);
    ~SysLogWriter();

    void Write(const std::string &arMsg, LogLevel aCurrentLevel) override;

protected:
    std::string mIdent;
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif /* SRC_LOGGING_SYSLOGWRITER_H_ */
