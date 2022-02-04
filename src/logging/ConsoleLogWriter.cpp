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

using namespace rsp::utils;

namespace rsp::logging {

class DefaultConsoleStream : public ConsoleLogStreamsInterface
{
public:
    void Error(const std::string &arMsg) override {
        std::cerr << arMsg;
    }

    void Info(const std::string &arMsg) override {
        std::cout << arMsg;
    }
};

ConsoleLogWriter::ConsoleLogWriter(std::string aAcceptLevel, ConsoleLogStreamsInterface *apConsole, const ConsoleColors_t *apColors)
    : mpConsole(apConsole),
	  mpColors(apColors)
{
    mAcceptLevel = ToLogLevel(aAcceptLevel);
}

ConsoleLogWriter::ConsoleLogWriter(LogLevel aAcceptLevel, ConsoleLogStreamsInterface *apConsole, const ConsoleColors_t *apColors)
    : mAcceptLevel(aAcceptLevel),
      mpConsole(apConsole),
	  mpColors(apColors)
{
}

logging::ConsoleLogWriter::~ConsoleLogWriter()
{
    if (mpConsole) {
        delete mpConsole;
    }
}

void ConsoleLogWriter::Write(const std::string &arMsg, LogLevel aCurrentLevel)
{
    if (!mpConsole) {
        mpConsole = new DefaultConsoleStream();
    }

    if (aCurrentLevel <= mAcceptLevel) {

    	std::string s;
    	if (mpColors) {
    		s = (*mpColors)[static_cast<int>(aCurrentLevel)] + arMsg + std::string(AnsiEscapeCodes::ec::ConsoleDefault);
    	}
    	else {
    		s = arMsg;
    	}

        if (aCurrentLevel < LogLevel::Warning) {
            mpConsole->Error(s);
        }
        else {
            mpConsole->Info(s);
        }
    }
}

} /* namespace logging */

