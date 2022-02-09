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


TestApplication::TestApplication(int argc, const char **argv)
    : ApplicationBase(argc, argv)
{
    mLogHandle = mLogger.AddLogWriter(std::make_shared<FileLogWriter>(mCmd.GetAppName() + ".log", LogLevel::Info));
}

TestApplication::~TestApplication()
{
    mLogger.RemoveLogWriter(mLogHandle);
}

void TestApplication::execute()
{
    if (!mCallback) {
        GetLog().Info() <<  mCmd.GetAppName() << " says \"Hello World.\"" << std::endl;
        Terminate(true);
    }
    else if (mCallback(*this)) {
        Terminate(true);
    }
}

void TestApplication::handleOptions()
{
    ApplicationBase::handleOptions();

    if ( mCmd.HasOption("-c") || mCmd.HasOption("--configure")) {
        Console::Info() << "Configuration prints here..." << std::endl;
    }
}

void TestApplication::showHelp()
{
    Console::Info() << "Usage: " << mCmd.GetAppName() << " --help\tShows this help" << std::endl;
}
