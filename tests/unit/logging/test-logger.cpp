/*
 * test-logger.cpp
 *
 *  Created on: Feb 22, 2020
 *      Author: Steffen Brummer
 */

#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <doctest.h>
#include <logging/Logger.h>
#include <logging/ConsoleLogWriter.h>
#include <logging/FileLogWriter.h>
#include <utils/StrUtils.h>
#include <utils/AnsiEscapeCodes.h>
#include <utils/CoreExceptions.h>

using namespace rsp;
using namespace rsp::utils;
using namespace rsp::logging;

const char *cFileName = "__logger-test.log";


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


TEST_CASE("Testing the Logger class") {

    std::remove(cFileName);
    mConsoleErrorBuffer.clear();
    mConsoleInfoBuffer.clear();

    logging::Logger log(true);

    CHECK_THROWS_AS(logging::LoggerInterface::GetDefault(), const utils::NotsetException &);
    logging::LoggerInterface::SetDefault(log);

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

    std::clog << LogLevel::Critical << "Critical to std::clog" << std::endl;

    auto end = std::chrono::high_resolution_clock::now() + std::chrono::seconds(1);
    auto tend = std::chrono::system_clock::to_time_t(end);
    log.Emergency() << "Sleeping until " << std::ctime(&tend) << std::flush;

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

    CHECK(StrUtils::EndsWith(line, "(critical) Critical to std::clog") == true);
    CHECK(StrUtils::StartsWith(mConsoleErrorBuffer[1], std::string(AnsiEscapeCodes::ec::fg::Red) + "Critical to std::clog") == true);

    std::getline(fin, line);
    CHECK(StrUtils::Contains(line, "(emergency) Sleeping until") == true);
    CHECK(StrUtils::StartsWith(mConsoleErrorBuffer[2], std::string(AnsiEscapeCodes::ec::fg::Red) + "Sleeping until") == true);

    for (int i = 0 ; i < 22 ; i++) {
        std::getline(fin, line);
        CHECK(StrUtils::Contains(line, "] (info) Writing from ") == true);
    }

    std::getline(fin, line);
    CHECK(StrUtils::EndsWith(line, "(info) Wakeup...") == true);

    std::getline(fin, line);
    CHECK(fin.eof() == true);

    CHECK(&(logging::LoggerInterface::GetDefault()) == &log);
}


