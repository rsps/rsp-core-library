/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <fstream>
#include <filesystem>
#include <doctest.h>
#include <application/CommandLine.h>
#include <utils/StrUtils.h>
#include <utils/CoreException.h>
#include "../../TestApplication.h"

using namespace rsp::application;

const char *cLogFileName = "MyApplication.log";

TEST_CASE("Application Instance")
{
    const char *arguments[] = {
        "./MyApplication",
        "-c",
        "--version",
        "--help"
    };

    SUBCASE("Instantiate CommandLine") {
        CommandLine cmd(4, arguments);
    }

    SUBCASE("Instantiate ApplicationBase") {
        ApplicationBase app;

        CHECK(app.GetCommandLine().GetOptions().size() == 0);
        CHECK(app.GetCommandLine().GetCommands().size() == 0);
    }

    SUBCASE("Instantiate TestApplication") {
        std::remove(cLogFileName);

        CHECK_THROWS_AS(ApplicationBase::Get<TestApplication>(), const rsp::utils::ENoInstance &);

        TestApplication app(2, arguments);

        ApplicationBase::Get<TestApplication>().Run();

        CHECK(std::filesystem::exists(std::filesystem::path(cLogFileName)));

        std::ifstream fin;
        fin.open(cLogFileName);
        CHECK(fin.is_open() == true);
        std::string line;
        std::getline(fin, line);
        CHECK(rsp::utils::StrUtils::EndsWith(line, "\"Hello World.\"") == true);
    }

    SUBCASE("Execute Callback") {
        TestApplication app(1, arguments);

        app.SetCallback([](TestApplication &arApp) -> bool {
            arApp.GetLog().Notice() << "Logged from callback." << std::endl;
            return true;
        });
        ApplicationBase::Get<TestApplication>().Run();

        std::ifstream fin;
        fin.open(cLogFileName);
        CHECK(fin.is_open() == true);
        std::string line;
        std::getline(fin, line);
        std::getline(fin, line);
        CHECK(rsp::utils::StrUtils::EndsWith(line, "Logged from callback.") == true);
    }

}


