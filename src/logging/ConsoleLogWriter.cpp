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
#include <string>
#include <sstream>
#include <utils/AnsiEscapeCodes.h>
#include <logging/ConsoleLogWriter.h>
#include <json/JsonEncoder.h>

using namespace rsp::utils;

namespace rsp::logging {

class DefaultConsoleStream : public ConsoleLogStreamsInterface
{
public:
    void Error(const std::string &arMsg) override {
        std::cerr << arMsg << std::endl;
    }

    void Info(const std::string &arMsg) override {
        std::cout << arMsg << std::endl;
    }
};

ConsoleLogWriter::ConsoleLogWriter(const std::string& arAcceptLevel, ConsoleLogStreamsInterface *apConsole, const ConsoleColors_t *apColors)
    : ConsoleLogWriter(ToLogLevel(arAcceptLevel), apConsole, apColors)
{
}

ConsoleLogWriter::ConsoleLogWriter(LogLevel aAcceptLevel, ConsoleLogStreamsInterface *apConsole, const ConsoleColors_t *apColors)
    : mpConsole(apConsole ? apConsole : new DefaultConsoleStream()),
      mpColors(apColors)
{
    mAcceptLevel = aAcceptLevel;
}

ConsoleLogWriter::~ConsoleLogWriter()
{
    delete mpConsole;
}

void ConsoleLogWriter::Write(const std::string &arMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext)
{
    if (!arMsg.length() || (mAcceptLevel < aCurrentLevel)) {
        return;
    }

    std::stringstream ss;
    if (mpColors) {
        ss << (*mpColors)[std::size_t(aCurrentLevel)];
    }
    if (arChannel.length()) {
        ss << arChannel << ": ";
    }
    ss << arMsg;
    if (!arContext.IsNull()) {
        ss << " " << rsp::json::JsonEncoder().Encode(arContext);
    }
    if (mpColors) {
        ss << std::string(AnsiEscapeCodes::ec::ConsoleDefault);
    }

    if (aCurrentLevel < LogLevel::Warning) {
        mpConsole->Error(ss.str()); // Write to std::cerr
    }
    else {
        mpConsole->Info(ss.str());
    }
}

} /* namespace logging */
