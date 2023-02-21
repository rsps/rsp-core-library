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
#include <time.h>
#include <doctest.h>
#include <logging/Logger.h>
#include <logging/ConsoleLogWriter.h>
#include <logging/FileLogWriter.h>
#include <utils/StrUtils.h>
#include <utils/AnsiEscapeCodes.h>
#include <utils/CoreException.h>

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
    os << arType.Value;
    return os;
}


static std::vector<std::string> mConsoleInfoBuffer;
static std::vector<std::string> mConsoleErrorBuffer;

class TestConsoleStream : public ConsoleLogStreamsInterface
{
public:
    void Error(const std::string &arMsg) override {
        mConsoleErrorBuffer.push_back(std::string(arMsg));
    }

    void Info(const std::string &arMsg) override {
        mConsoleInfoBuffer.push_back(std::string(arMsg));
    }
};

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


TEST_CASE("Logging") {

    std::remove(cFileName);
    mConsoleErrorBuffer.clear();
    mConsoleInfoBuffer.clear();

    logging::Logger log(true);

    CHECK_THROWS_AS(logging::LoggerInterface::GetDefault(), const utils::NotSetException &);
    CHECK_NOTHROW(logging::LoggerInterface::SetDefault(&log));

    CHECK_NOTHROW(log.SetChannel("Test Channel"));
    CHECK_NOTHROW(log.SetContext(DynamicData().Add("Test Context").Add(42)));

    CHECK_NOTHROW(log.AddLogWriter(std::make_shared<logging::FileLogWriter>(cFileName, logging::LogLevel::Info)));
    CHECK_NOTHROW(log.AddLogWriter(std::make_shared<logging::ConsoleLogWriter>(logging::LogLevel::Critical, new TestConsoleStream(), &cConsoleColors)));

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
    CHECK_NOTHROW(log.Debug() << LogLevel::Info << "Dbg-Info");

    MyType type;
    CHECK_NOTHROW(log.Info() << type);

    CHECK_NOTHROW(std::clog << LogLevel::Critical << "Critical to std::clog" << std::endl);

    CHECK_NOTHROW(log.Emergency() << "Sleeping for 1 second");
    auto end = std::chrono::high_resolution_clock::now() + std::chrono::seconds(1);

    std::thread t([&]() {
        for (int i=0; i < 12 ; i++) {
            CHECK_NOTHROW(std::clog << LogLevel::Info << "Writing from thread " << i << std::endl);
            std::this_thread::sleep_for(std::chrono::milliseconds(90));
        }
    });

    do {
        CHECK_NOTHROW(std::clog << LogLevel::Info << "Writing from main" << std::endl);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while (std::chrono::high_resolution_clock::now() < end);
    CHECK_NOTHROW(std::clog << "Wakeup..." << std::endl);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    t.join();

    std::ifstream fin;
    fin.open(cFileName);

    CHECK(fin.is_open() == true);

    std::string line;

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "Test of logger") == true);

    CHECK(mConsoleErrorBuffer.size() == 3);
    CHECK(mConsoleInfoBuffer.size() == 0);

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "] <Test Channel> (alert) Alert  [\"Test Context\",42]"));

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "Error") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "Warning") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "Info") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "Debug") == false);
    CHECK(StrUtils::Contains(line, "Dbg-Info") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "666") == true);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::EndsWith(line, "(critical) Critical to std::clog") == true, line);
    CHECK(StrUtils::Contains(mConsoleErrorBuffer[1], std::string(AnsiEscapeCodes::ec::fg::Red) + "Critical to std::clog") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "(emergency) Sleeping for 1 second") == true);
    CHECK(StrUtils::StartsWith(mConsoleErrorBuffer[2], std::string(AnsiEscapeCodes::ec::fg::Red) + "<Test Channel> Sleeping for 1 second") == true);

    for (int i = 0 ; i < 22 ; i++) {
        std::getline(fin, line);
        CHECK(StrUtils::Contains(line, "] (info) Writing from ") == true);
    }

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "(info) Wakeup...") == true);

    std::getline(fin, line);
    CHECK(fin.eof() == true);

    CHECK(&(logging::LoggerInterface::GetDefault()) == &log);
}


