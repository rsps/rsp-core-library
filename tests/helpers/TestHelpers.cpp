/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <logging/ConsoleLogWriter.h>
#include <posix/FileIO.h>
#include "TestHelpers.h"

using namespace rsp::logging;

rsp::logging::LogLevel TestHelpers::mLogLevel = rsp::logging::LogLevel::Notice;

void TestHelpers::AddConsoleLogger(Logger& arLogger)
{
    LoggerInterface::SetDefault(&arLogger);
    arLogger.AddLogWriter(std::make_shared<rsp::logging::ConsoleLogWriter>(mLogLevel));
}

std::uint8_t TestHelpers::TamperWithFile(const std::string &arFileName, std::size_t aOffset, std::uint8_t aValue)
{
    std::uint8_t result;
    rsp::posix::FileIO f(arFileName, std::ios_base::in | std::ios_base::out);
    f.Seek(aOffset);
    f.ExactRead(&result, sizeof(result));
    f.Seek(aOffset);
    f.ExactWrite(&aValue, sizeof(aValue));
    return result;
}

void TestHelpers::ParseArguments(const char **apArgv)
{
    for(; *apArgv; ++apArgv) {
        if(strncmp(*apArgv, "-vv", strlen("-vv")) == 0) {
            mLogLevel = rsp::logging::LogLevel::Debug;
        }
        else if(strncmp(*apArgv, "-v", strlen("-v")) == 0) {
            mLogLevel = rsp::logging::LogLevel::Info;
        }
    }
}
