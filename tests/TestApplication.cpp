/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <logging/FileLogWriter.h>
#include "TestApplication.h"

const char *cLogFileName = "rsp-core-test.log";

TestApplication::TestApplication(rsp::application::CommandLine &aCmd)
    : CliApplication(aCmd)
{
    mLogHandle = mLogger.AddLogWriter(std::make_shared<rsp::logging::FileLogWriter>(cLogFileName, rsp::logging::LogLevel::Info));
}

TestApplication::~TestApplication()
{
    mLogger.RemoveLogWriter(mLogHandle);
}

void TestApplication::execute()
{
    GetLog().Info() << "\"Hello World.\" says " << mCmd.GetAppName() << std::endl;
}

void TestApplication::handleOptions()
{
}

void TestApplication::showHelp()
{
}
