/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_LOGGING_FILE_LOG_WRITER_H
#define RSP_CORE_LIB_LOGGING_FILE_LOG_WRITER_H

#include <logging/LogWriterInterface.h>
#include <fstream>
#include <mutex>

namespace rsp::logging {

/**
 * \class FileLogWriter
 *
 * \brief A file based log writer
 */
class FileLogWriter : public LogWriterInterface {
public:
    /**
     * Construct a log writer with accept level given as text.
     *
     * \param aFileName
     * \param aAcceptLevel
     */
    FileLogWriter(const std::string& arFileName, const std::string& arAcceptLevel);

    /**
     * Construct a log writer with accept level given as LogLevel type.
     *
     * \param aFileName
     * \param aAcceptLevel
     */
    FileLogWriter(const std::string& arFileName, LogLevel aAcceptLevel);
    ~FileLogWriter() override;


    void Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext) override;

protected:
    std::ofstream mOutput;
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif // RSP_CORE_LIB_LOGGING_FILE_LOG_WRITER_H
