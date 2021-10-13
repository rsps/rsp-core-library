/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_LOGGING_FILELOGWRITER_H_
#define SRC_LOGGING_FILELOGWRITER_H_

#include <fstream>
#include <mutex>
#include "LogWriter.h"

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
    FileLogWriter(std::string aFileName, std::string aAcceptLevel);

    /**
     * Construct a log writer with accept level given as LogLevel type.
     *
     * \param aFileName
     * \param aAcceptLevel
     */
    FileLogWriter(std::string aFileName, LogLevel aAcceptLevel);
    ~FileLogWriter() override;


    void Write(const std::string &arMsg, LogLevel aCurrentLevel) override;

protected:
    std::ofstream mOutput;
    LogLevel mAcceptLevel;
};

} /* namespace logging */

#endif /* SRC_LOGGING_FILELOGWRITER_H_ */
