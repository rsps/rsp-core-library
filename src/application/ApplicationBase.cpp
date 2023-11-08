/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <application/ApplicationBase.h>
#include <application/Console.h>
#include <logging/ConsoleLogWriter.h>
#include <logging/FileLogWriter.h>
#include <logging/SysLogWriter.h>
#include <version.h>

using namespace rsp::exceptions;
using namespace rsp::logging;
using namespace rsp::utils;

namespace rsp::application {

ApplicationBase* ApplicationBase::mpInstance = nullptr;


ApplicationBase::ApplicationBase(int argc, const char **argv, const char *apAppName)
    : mAppName(apAppName ? apAppName : std::string()),
      mLogger(true),
      mCmd(argc, argv)
{
    if (mpInstance) {
        THROW_WITH_BACKTRACE1(rsp::exceptions::ESingletonViolation, "ApplicationBase");
    }

    installLogWriters();
    LoggerInterface::SetDefault(&mLogger);

    mpInstance = this;
}

ApplicationBase::~ApplicationBase()
{
    LoggerInterface::SetDefault(static_cast<rsp::logging::LoggerInterface*>(nullptr));
    mpInstance = nullptr;
}

int ApplicationBase::Run()
{
    try {
        beforeExecute();

        while(!mTerminated) {
            try {
                execute();
            }
            catch(const ETerminate &e) {
                mTerminated = true;
                mApplicationResult = e.GetCode();
            }
            catch(const std::exception &e) {
                mLogger.Critical() << "Unhandled exception: " << e.what() << " Attempting graceful shutdown" << std::endl;
                mApplicationResult = cResultUnhandledError;
                mTerminated = true;
            }
        }
    }
    catch(const ETerminate &e) {
        mTerminated = true;
        mApplicationResult = e.GetCode();
    }

    afterExecute();

    return mApplicationResult;
}

void ApplicationBase::beforeExecute()
{
    handleOptions();
}

void ApplicationBase::handleOptions()
{
    if ( mCmd.HasOption("-h") || mCmd.HasOption("--help")) {
        showHelp();
        THROW_WITH_BACKTRACE1(ETerminate, cResultSuccess);
    }

    if ( mCmd.HasOption("--version")) {
        showVersion();
        THROW_WITH_BACKTRACE1(ETerminate, cResultSuccess);
    }
}

void ApplicationBase::showHelp()
{
    Console::Info() << "No help text available.";
}

void ApplicationBase::showVersion()
{
    Console::Info() << "Library version: " << get_library_version();
}

void ApplicationBase::installLogWriters()
{
    auto level = LogLevel::Notice;
    if (mCmd.HasOption("-vv") || mCmd.HasOption("-vvv")) {
        level = LogLevel::Debug;
    }
    else if (mCmd.HasOption("-v")) {
        level = LogLevel::Info;
    }
    std::string l;
    if (mCmd.GetOptionValue("--loglevel=", l)) {
        level = ToLogLevel(l);
    }

    std::string s;
    if (mCmd.GetOptionValue("--log=", s)) {
        if (s == "syslog") {
            mLogWriter = mLogger.MakeLogWriter<SysLogWriter>(GetAppName(), level, LogType::User);
        }
        else if (s == "console") {
            mLogWriter = mLogger.MakeLogWriter<ConsoleLogWriter>(level);
        }
        else {
            mLogWriter = mLogger.MakeLogWriter<FileLogWriter>(s, level);
        }
    }
    else {
        mLogWriter = mLogger.MakeLogWriter<ConsoleLogWriter>(level);
    }
}

const std::string &ApplicationBase::GetAppName() const
{
    if (mAppName.empty()) {
        return mCmd.GetAppName();
    }
    return mAppName;
}

} /* namespace rsp::application */
