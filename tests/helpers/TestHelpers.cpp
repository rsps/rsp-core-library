/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <string>
#include <rsp/posix/FileIO.h>
#include <rsp/posix/FileSystem.h>
#include <rsp/utils/HexStream.h>
#include "TestHelpers.h"

uint8_t TestHelpers::TamperWithFile(const std::string& arFileName, uint32_t aOffset, uint8_t aValue)
{
    uint8_t result;
    rsp::posix::FileIO f(arFileName, std::ios_base::in | std::ios_base::out);
    f.Seek(aOffset);
    f.ExactRead(&result, sizeof(result));
    f.Seek(aOffset);
    f.ExactWrite(&aValue, sizeof(aValue));
    return result;
}

void TestHelpers::ParseArguments(const char** apArgv)
{
    for (; *apArgv; ++apArgv) {
        if (strncmp(*apArgv, "-vvv", strlen("-vvv")) == 0) {
            TestLogger::mLogLevel = rsp::logging::LogLevel::Debug;
        }
        else if (strncmp(*apArgv, "-vv", strlen("-vv")) == 0) {
            TestLogger::mLogLevel = rsp::logging::LogLevel::Info;
        }
    }
}

std::string TestHelpers::ToHex(const std::string& arString)
{
    return rsp::utils::ToHex(arString);
}

std::string TestHelpers::ToHex(uint32_t aValue)
{
    return rsp::utils::ToHex(aValue);
}

std::string TestHelpers::ToHex(const uint8_t* apData, uint32_t aSize, uint32_t aSizeOf)
{
    return rsp::utils::ToHex(apData, aSize, aSizeOf);
}

bool TestHelpers::ValidateJson(const std::string& arJson)
{
    std::ofstream fout("/tmp/ValidateJson.json");
    fout << arJson;
    fout.close();

    int result = 1;
    if (rsp::posix::FileSystem::FileExists("/usr/bin/jsonlint-php")) {
        result = std::system("/usr/bin/jsonlint-php --quiet /tmp/ValidateJson.json");
    }

    std::remove("/tmp/ValidateJson.json");

    return (result == 0);
}

bool TestHelpers::ValidateJsonFile(const std::string& arJsonFile)
{
    int result = std::system((std::string("/usr/bin/jsonlint-php --quiet ") + arJsonFile).c_str());

    return (result == 0);
}

int TestHelpers::StartWebServer()
{
    // Find lighttpd executable
    static constexpr std::string_view PATHS[] = {
            "/usr/sbin/lighttpd", "/usr/bin/lighttpd",
            "/usr/local/sbin/lighttpd", "/usr/local/bin/lighttpd"};

    std::string_view exe = [] {
        for (const auto& p : PATHS) {
            if (std::filesystem::exists(p)) {
                return p;
            }
        }
        return std::string_view{};
    }();

    // Fall back to searching PATH
    if (exe.empty()) {
        exe = "lighttpd";
    }

    [[maybe_unused]] int rc = std::system("killall lighttpd -q"); // Make sure it is not running
    std::string cwd = std::filesystem::current_path();
    std::string command = std::format("LIGHTTPD_BASEDIR='{}/webserver' {} -f '{}/webserver/lighttpd.conf'", cwd, exe, cwd);
    return std::system(command.c_str());
}

int TestHelpers::StopWebServer()
{
    return std::system("killall lighttpd -q");
}
