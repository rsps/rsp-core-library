/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <memory>
#include <application/CliApplication.h>
#include <application/Console.h>
#include <logging/FileLogWriter.h>
#include <version.h>

namespace rsp::application {

CliApplication::CliApplication(CommandLine &aCmd)
    : mCmd(aCmd)
{
}

int CliApplication::Run()
{
    handleOptions();

    return ApplicationBase::Run();
}

void CliApplication::handleOptions()
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

void CliApplication::showHelp()
{
    Console::Info() << "No help text available." << std::endl;
}

void CliApplication::showVersion()
{
    Console::Info() << "Library version: " << get_library_version() << std::endl;
}


} /* namespace rsp::application */
