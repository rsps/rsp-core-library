/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <utils/DateTime.h>
#include <logging/FileLogWriter.h>

using namespace rsp::utils;

namespace rsp::logging {

FileLogWriter::FileLogWriter(std::string aFileName, std::string aAcceptLevel)
    : mOutput(aFileName, std::ios_base::out | std::ios_base::app),
      mAcceptLevel(ToLogLevel(aAcceptLevel))
{
}

FileLogWriter::FileLogWriter(std::string aFileName, LogLevel aAcceptLevel)
    : mOutput(aFileName, std::ios_base::out | std::ios_base::app),
      mAcceptLevel(aAcceptLevel)
{
}

FileLogWriter::~FileLogWriter()
{
    mOutput.close();
}

void FileLogWriter::Write(const std::string &arMsg, LogLevel aCurrentLevel)
{
    if (arMsg.length() && (mAcceptLevel >= aCurrentLevel)) {
        DateTime dt;
        mOutput << "[" << dt.ToLogging() << "] (" << ToString(aCurrentLevel) << ") " << arMsg << std::flush;
    }
}


} /* namespace logging */

