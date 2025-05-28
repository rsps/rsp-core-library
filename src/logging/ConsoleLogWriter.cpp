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
    if (arMsg.empty() || (mAcceptLevel < aCurrentLevel)) {
        return;
    }

    std::string context;
    if (!arContext.IsNull()) {
        context = rsp::json::JsonEncoder().Encode(arContext);
    }

    std::string s;
    s.reserve(arMsg.size() + context.size() + 20);
    if (mpColors) {
        s += (*mpColors)[std::size_t(aCurrentLevel)];
    }
    if (!arChannel.empty()) {
        s += arChannel;
        s += ": ";
    }
    s += arMsg;
    if (!context.empty()) {
        s += " ";
        s += context;
    }
    if (mpColors) {
        s += std::string(AnsiEscapeCodes::ec::ConsoleDefault);
    }

    if (aCurrentLevel < LogLevel::Warning) {
        mpConsole->Error(s); // Write to std::cerr
    }
    else {
        mpConsole->Info(s);
    }
}

} /* namespace logging */
