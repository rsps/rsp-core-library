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
#include <utils/AnsiEscapeCodes.h>
#include <logging/ConsoleLogWriter.h>
#include <json/JsonEncoder.h>

using namespace rsp::utils;

namespace rsp::logging {

class DefaultConsoleStream : public ConsoleLogStreamsInterface
{
public:
    void Write(std::string_view aMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext, const std::string& arColor) override
    {
        auto &out = (aCurrentLevel < LogLevel::Warning) ? std::cout : std::cerr;

        if (!arColor.empty()) {
            out << arColor;
        }
        if (!arChannel.empty()) {
            out << arChannel << ": ";
        }
        out << aMsg;
        if (!arContext.IsNull()) {
            out << " " << rsp::json::JsonEncoder().Encode(arContext);
        }
        if (!arColor.empty()) {
            out << std::string(AnsiEscapeCodes::ec::ConsoleDefault);
        }
        out << std::endl;
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

void ConsoleLogWriter::Write(std::string_view aMsg, LogLevel aCurrentLevel, const std::string &arChannel, const rsp::utils::DynamicData &arContext)
{
    if (aMsg.empty() || (mAcceptLevel < aCurrentLevel)) {
        return;
    }
    mpConsole->Write(aMsg, aCurrentLevel, arChannel, arContext, (mpColors ? (*mpColors)[std::size_t(aCurrentLevel)] : std::string()));
}

} /* namespace logging */
