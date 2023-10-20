/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_SYS_LOG_WRITER_H
#define RSP_CORE_LIB_LOGGING_SYS_LOG_WRITER_H

#include <logging/LogWriterInterface.h>

namespace rsp::logging {

/**
 * \class SysLogWriter
 * \brief A log writer that sends output to a syslog server.
 */
class SysLogWriter: public LogWriterInterface {
public:
    SysLogWriter(std::string aIdent, const std::string& arAcceptLevel, LogType aType);
    SysLogWriter(std::string aIdent, LogLevel aAcceptLevel, LogType aType);
    ~SysLogWriter() override;

    void Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext) override;

protected:
    std::string mIdent;
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif // RSP_CORE_LIB_LOGGING_SYS_LOG_WRITER_H
