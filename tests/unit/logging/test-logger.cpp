/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <doctest.h>
#include <exceptions/CoreException.h>
#include <logging/Logger.h>
#include <logging/ConsoleLogWriter.h>
#include <logging/FileLogWriter.h>
#include <utils/StrUtils.h>
#include <utils/AnsiEscapeCodes.h>

using namespace rsp;
using namespace rsp::utils;
using namespace rsp::logging;

const char *cFileName = "__logger-test.log";

struct MyType {
    int Value = 666;
};

std::ostream& operator<< (std::ostream& os, const MyType &arType);

std::ostream& operator<< (std::ostream& os, const MyType &arType)
{
    os << "MyType: " << arType.Value;
    return os;
}


static std::vector<std::string> mConsoleInfoBuffer;
static std::vector<std::string> mConsoleErrorBuffer;

class TestConsoleStream : public ConsoleLogStreamsInterface
{
public:
    void Error(const std::string &arMsg) override {
        mConsoleErrorBuffer.emplace_back(arMsg);
    }

    void Info(const std::string &arMsg) override {
        mConsoleInfoBuffer.emplace_back(arMsg);
    }
};

TEST_CASE("Logging") {

    const rsp::logging::ConsoleLogWriter::ConsoleColors_t cConsoleColors {
            AnsiEscapeCodes::ec::fg::Red,       // Emergency
            AnsiEscapeCodes::ec::fg::Red,       // Alert
            AnsiEscapeCodes::ec::fg::Red,       // Critical
            AnsiEscapeCodes::ec::fg::Red,       // Error
            AnsiEscapeCodes::ec::fg::Yellow,    // Warning
            AnsiEscapeCodes::ec::fg::Cyan,      // Notice
            AnsiEscapeCodes::ec::fg::LightBlue, // Info
            AnsiEscapeCodes::ec::fg::LightGreen // Debug
    };

    std::remove(cFileName);
    mConsoleErrorBuffer.clear();
    mConsoleInfoBuffer.clear();

    logging::Logger log(true);

    CHECK_THROWS_AS(logging::LoggerInterface::GetDefault(), const exceptions::NotSetException &);
    CHECK_NOTHROW(logging::LoggerInterface::SetDefault(&log));

    CHECK_NOTHROW(log.SetChannel("Test Channel"));
    CHECK_NOTHROW(log.SetContext(DynamicData().Add("Test Context").Add(42)));

    CHECK_NOTHROW(auto h = log.AddLogWriter(std::make_shared<logging::FileLogWriter>(cFileName, logging::LogLevel::Info)));
    CHECK_NOTHROW(auto h = log.AddLogWriter(std::make_shared<logging::ConsoleLogWriter>(logging::LogLevel::Critical, new TestConsoleStream(), &cConsoleColors)));

    CHECK_NOTHROW(log.Info() << "Test of logger");
    CHECK_NOTHROW(std::this_thread::sleep_for(std::chrono::milliseconds(7)));
    CHECK_NOTHROW(log.Alert() << "Alert");
    CHECK_NOTHROW(std::this_thread::sleep_for(std::chrono::milliseconds(2)));
    CHECK_NOTHROW(log.Error() << "Error");
    CHECK_NOTHROW(std::this_thread::sleep_for(std::chrono::milliseconds(3)));
    CHECK_NOTHROW(log.Warning() << "Warning");
    CHECK_NOTHROW(std::this_thread::sleep_for(std::chrono::milliseconds(5)));
    CHECK_NOTHROW(log.Info() << "Info");
    CHECK_NOTHROW(std::this_thread::sleep_for(std::chrono::milliseconds(4)));
    CHECK_NOTHROW(log.Debug() << "Debug");
    CHECK_NOTHROW(log.Debug() << SetLevel(LogLevel::Info) << "Dbg-Info");

    MyType type;
    CHECK_NOTHROW(log.Info() << type);

    CHECK_NOTHROW(std::clog << SetLevel(LogLevel::Critical) << "Critical to std::clog" << std::endl);

    CHECK_NOTHROW(log.Emergency() << "Sleeping for 1 second");
    auto end = std::chrono::high_resolution_clock::now() + std::chrono::seconds(1);

    std::thread t([&]() {
        for (int i=0; i < 12 ; i++) {
            CHECK_NOTHROW(std::clog << SetLevel(LogLevel::Info) << "Writing from thread " << i << std::endl);
            std::this_thread::sleep_for(std::chrono::milliseconds(90));
        }
    });

    do {
        CHECK_NOTHROW(std::clog << SetLevel(LogLevel::Info) << "Writing from main" << std::endl);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while (std::chrono::high_resolution_clock::now() < end);
    CHECK_NOTHROW(std::clog << "Wakeup..." << std::endl);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    t.join();

    std::ifstream fin;
    fin.open(cFileName);

    CHECK_EQ(fin.is_open(), true);

    std::string line;

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "Test of logger"), line);

    CHECK_EQ(mConsoleErrorBuffer.size(), 3);
    CHECK_EQ(mConsoleInfoBuffer.size(), 0);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::EndsWith(line, "] <Test Channel> (Alert) Alert  [\"Test Context\",42]"), line);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "Error"), line);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "Warning"), line);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "Info"), line);

    std::getline(fin, line);
    CHECK_FALSE_MESSAGE(StrUtils::Contains(line, "Debug"), line);
    CHECK_MESSAGE(StrUtils::Contains(line, "Dbg-Info") , line);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "MyType: 666"), line);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::EndsWith(line, "(Critical) Critical to std::clog"), line);
    CHECK_MESSAGE(StrUtils::Contains(mConsoleErrorBuffer[1], std::string(AnsiEscapeCodes::ec::fg::Red) + "Critical to std::clog"), mConsoleErrorBuffer[1]);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "(Emergency) Sleeping for 1 second"), line);
    CHECK_MESSAGE(StrUtils::StartsWith(mConsoleErrorBuffer[2], std::string(AnsiEscapeCodes::ec::fg::Red) + "<Test Channel> Sleeping for 1 second"), mConsoleErrorBuffer[2]);

    for (int i = 0 ; i < 22 ; i++) {
        std::getline(fin, line);
        CHECK_MESSAGE(StrUtils::Contains(line, "] (Info) Writing from "), line);
    }

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::Contains(line, "(Info) Wakeup..."), line);

    std::getline(fin, line);
    CHECK_MESSAGE(fin.eof(), line);

    CHECK_EQ(&(logging::LoggerInterface::GetDefault()), &log);

    CHECK_NOTHROW(logging::LoggerInterface::SetDefault(nullptr));
}
