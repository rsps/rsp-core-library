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
#include <application/Console.h>
#include "TestApplication.h"

using namespace rsp::application;
using namespace rsp::logging;


TestApplication::TestApplication(rsp::application::CommandLine &aCmd)
    : CliApplication(aCmd)
{
    mLogHandle = mLogger.AddLogWriter(std::make_shared<FileLogWriter>(aCmd.GetAppName() + ".log", LogLevel::Info));
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
    CliApplication::handleOptions();

    if ( mCmd.HasOption("-c") || mCmd.HasOption("--configure")) {
        Console::Info() << "Configuration prints here..." << std::endl;
    }
}

void TestApplication::showHelp()
{
    Console::Info() << "Usage: " << mCmd.GetAppName() << " --help\tShows this help" << std::endl;
}
