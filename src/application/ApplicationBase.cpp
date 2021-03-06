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
#include <logging/FileLogWriter.h>
#include <version.h>

using namespace rsp::utils;

namespace rsp::application {

ApplicationBase* ApplicationBase::mpInstance = nullptr;


ApplicationBase::ApplicationBase(int argc, const char **argv)
    : mLogger(true),
      mCmd(argc, argv)
{
    if (mpInstance) {
        THROW_WITH_BACKTRACE(ESingletonViolation);
    }

    rsp::logging::LoggerInterface::SetDefault(&mLogger);

    mpInstance = this;
}

ApplicationBase::~ApplicationBase()
{
    rsp::logging::LoggerInterface::SetDefault(static_cast<rsp::logging::LoggerInterface*>(nullptr));

    mpInstance = nullptr;
}


int ApplicationBase::Run()
{
    beforeExecute();
    while(!mTerminated) {
        try {
            execute();
        }
        catch(const std::exception &e) {
            mLogger.Critical() << "Unhandled exception: " << e.what() << std::endl;
            mApplicationResult = cResultUnhandledError;
            mTerminated = true;
        }
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
    std::string s;
    if (mCmd.GetOptionValue("--log=", s)) {
        std::string l;
        if (!mCmd.GetOptionValue("--loglevel=", l)) {
            l = ToString(rsp::logging::LogLevel::Info);
        }

        mLogger.AddLogWriter(std::make_shared<logging::FileLogWriter>(s, l));
    }

    if ( mCmd.HasOption("-h") || mCmd.HasOption("--help")) {
        showHelp();
        mTerminated = true;
    }

    if ( mCmd.HasOption("--version")) {
        showVersion();
        mTerminated = true;
    }
}

void ApplicationBase::showHelp()
{
    Console::Info() << "No help text available." << std::endl;
}

void ApplicationBase::showVersion()
{
    Console::Info() << "Library version: " << get_library_version() << std::endl;
}
} /* namespace rsp::application */
