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
#include <json/JsonEncoder.h>
#include <logging/FileLogWriter.h>
#include <utils/DateTime.h>
#include <utils/StrUtils.h>

using namespace rsp::utils;

namespace rsp::logging {

FileLogWriter::FileLogWriter(const std::string& arFileName, const std::string& arAcceptLevel)
    : mOutput(arFileName, std::ios_base::out | std::ios_base::app),
      mAcceptLevel(ToLogLevel(arAcceptLevel))
{
}

FileLogWriter::FileLogWriter(const std::string& arFileName, LogLevel aAcceptLevel)
    : mOutput(arFileName, std::ios_base::out | std::ios_base::app),
      mAcceptLevel(aAcceptLevel)
{
}

FileLogWriter::~FileLogWriter()
{
    mOutput.close();
}

void FileLogWriter::Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext)
{
    if (arMsg.length() && (mAcceptLevel >= aCurrentLevel)) {
        DateTime dt;
        mOutput << "[" << dt.ToLogging() << "] ";
        if (arChannel.length()) {
            mOutput << arChannel;
        }
        mOutput << "." << StrUtils::ToUpper(ToString(aCurrentLevel)) << ": " << arMsg;
        if (!arContext.IsNull()) {
            mOutput << " " << rsp::json::JsonEncoder().Encode(arContext);
        }
        mOutput << std::endl;
    }
}


} /* namespace logging */
