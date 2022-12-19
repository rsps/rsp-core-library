/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef TESTS_HELPERS_TESTHELPERS_H_
#define TESTS_HELPERS_TESTHELPERS_H_

#include <string>
#include <logging/Logger.h>

class TestHelpers
{
public:
    static void AddConsoleLogger(rsp::logging::Logger& arLogger);

    static std::uint8_t TamperWithFile(const std::string& arFileName, std::size_t aOffset, std::uint8_t aValue);

    static void ParseArguments(const char ** apArgv);

    static std::string ToHex(const std::string &arString);

    static bool ValidateJson(const std::string &arJson);
    static bool ValidateJsonFile(const std::string &arJsonFile);

protected:
    static rsp::logging::LogLevel mLogLevel;
};

#endif /* TESTS_HELPERS_TESTHELPERS_H_ */
