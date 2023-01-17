/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
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
    int Value = 42;
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
    logging::LoggerInterface::SetDefault(&log);

    log.AddLogWriter(std::make_shared<logging::FileLogWriter>(cFileName, logging::LogLevel::Info));
    log.AddLogWriter(std::make_shared<logging::ConsoleLogWriter>(logging::LogLevel::Critical, new TestConsoleStream(), &cConsoleColors));

    log.Info() << "Test of logger" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(7));
    log.Alert() << "Alert" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    log.Error() << "Error" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    log.Warning() << "Warning" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    log.Info() << "Info" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(4));
    log.Debug() << "Debug" << std::endl;

    MyType type;
    log.Info() << type << std::endl;

    std::clog << LogLevel::Critical << "Critical to std::clog" << std::endl;

    log.Emergency() << "Sleeping for 1 second" << std::flush;
    auto end = std::chrono::high_resolution_clock::now() + std::chrono::seconds(1);

    std::thread t([&]() {
        for (int i=0; i < 12 ; i++) {
            std::clog << LogLevel::Info << "Writing from tread " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(90));
        }
    });

    do {
        std::clog << LogLevel::Info << "Writing from main" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    while (std::chrono::high_resolution_clock::now() < end);
    std::clog << "Wakeup..." << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    t.join();

    std::ifstream fin;
    fin.open(cFileName);

    CHECK(fin.is_open() == true);

    std::string line;

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "Test of logger") == true);

    CHECK(mConsoleErrorBuffer.size() == 3);
    CHECK(mConsoleInfoBuffer.size() == 0);

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "Alert") == true);
    CHECK(StrUtils::StartsWith(mConsoleErrorBuffer[0], std::string(AnsiEscapeCodes::ec::fg::Red) + "Alert") == true);

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "Error") == true);

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "Warning") == true);

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "Info") == true);

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "Debug") == false);

    CHECK(StrUtils::EndsWith(line, "42") == true);

    std::getline(fin, line);
    CHECK_MESSAGE(StrUtils::EndsWith(line, "(critical) Critical to std::clog") == true, line);
    CHECK(StrUtils::StartsWith(mConsoleErrorBuffer[1], std::string(AnsiEscapeCodes::ec::fg::Red) + "Critical to std::clog") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "(emergency) Sleeping for 1 second") == true);
    CHECK(StrUtils::StartsWith(mConsoleErrorBuffer[2], std::string(AnsiEscapeCodes::ec::fg::Red) + "Sleeping for 1 second") == true);

    for (int i = 0 ; i < 21 ; i++) {
        std::getline(fin, line);
        CHECK(StrUtils::Contains(line, "] (info) Writing from ") == true);
    }

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "(info) Wakeup...") == true);

    std::getline(fin, line);
    CHECK(fin.eof() == true);

    CHECK(&(logging::LoggerInterface::GetDefault()) == &log);
}


