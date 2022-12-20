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
#include <string>
#include <sstream>
#include <iomanip>
#include <logging/ConsoleLogWriter.h>
#include <posix/FileIO.h>
#include "TestHelpers.h"

using namespace rsp::logging;

rsp::logging::LogLevel TestHelpers::mLogLevel = rsp::logging::LogLevel::Notice;

void TestHelpers::AddConsoleLogger(Logger& arLogger)
{
//    std::cout << "Adding Console logger with level " << ToString(mLogLevel) << std::endl;
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
        if(strncmp(*apArgv, "-vvv", strlen("-vvv")) == 0) {
            mLogLevel = rsp::logging::LogLevel::Debug;
        }
        else if(strncmp(*apArgv, "-vv", strlen("-vv")) == 0) {
            mLogLevel = rsp::logging::LogLevel::Info;
        }
    }
}

std::string TestHelpers::ToHex(const std::string &arString)
{
    std::stringstream out;
    std::string delim = ", ";
    auto sz = arString.size();
    std::string line;

    for (std::size_t i = 0 ; i < sz ; i++) {
        if ((i % 16) == 0) {
            out << "    ";
        }
        if (i == (sz - 1)) {
            delim = "";
        }
        if (arString[i] >= '0' && arString[i] <= 'z') {
            line += arString[i];
        }
        else {
            line += '.';
        }

        out << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(std::uint8_t(arString[i])) << delim;

        if ((i % 16) == 15) {
            out << "  " << line << "\n";
            line.clear();
        }
    }
    if ((sz % 16) != 15) {
        out << "  " << line << "\n";
    }
    out << std::dec;

    return out.str();
}

bool TestHelpers::ValidateJson(const std::string &arJson)
{
    std::ofstream fout("/tmp/ValidateJson.json");
    fout << arJson;
    fout.close();

    int result = std::system("/usr/bin/jsonlint-php /tmp/ValidateJson.json");

    std::remove("/tmp/ValidateJson.json");

    return (result == 0);
}

bool TestHelpers::ValidateJsonFile(const std::string &arJsonFile)
{
    int result = std::system((std::string("/usr/bin/jsonlint-php ") + arJsonFile).c_str());

    return (result == 0);
}

