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
#include <exceptions/CoreException.h>
#include <utils/StrUtils.h>
#include <utils/Function.h>
#include "../../helpers/TestApplication.h"

using namespace rsp::application;

const char *cLogFileName = "MyApplication.log";

TEST_CASE("Application")
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

        CHECK_EQ(app.GetCommandLine().GetOptions().size(), 0);
        CHECK_EQ(app.GetCommandLine().GetCommands().size(), 0);
    }

    SUBCASE("Instantiate TestApplication") {
        std::remove(cLogFileName);

        CHECK_THROWS_AS(ApplicationBase::Get<TestApplication>(), const rsp::exceptions::ENoInstance &);

        TestApplication app(2, arguments);

        ApplicationBase::Get<TestApplication>().Run();

        CHECK(std::filesystem::exists(std::filesystem::path(cLogFileName)));

        std::ifstream fin;
        fin.open(cLogFileName);
        CHECK_EQ(fin.is_open(), true);
        std::string line;
        std::getline(fin, line);
        CHECK_EQ(rsp::utils::StrUtils::EndsWith(line, "\"Hello World.\""), true);
    }

    SUBCASE("Execute Callback") {
        TestApplication app(1, arguments);

        app.SetCallback([](TestApplication &arApp) -> bool {
            arApp.GetLog().Notice() << "Logged from callback.";
            return true;
        });
        ApplicationBase::Get<TestApplication>().Run();

        std::ifstream fin;
        fin.open(cLogFileName);
        CHECK_EQ(fin.is_open(), true);
        std::string line;
        std::getline(fin, line);
        std::getline(fin, line);
        CHECK_EQ(rsp::utils::StrUtils::EndsWith(line, "Logged from callback."), true);
    }

    SUBCASE("Execute Callback to member function") {
        class MyClass {
        public:
            bool MyFunction(TestApplication &arApp) { // NOLINT, signature must match expected in callback
                arApp.GetLog().Notice() << "Logged from callback to member function.";
                return true;
            }
        };

        MyClass cls;
        TestApplication app(1, arguments);

        app.SetCallback(rsp::utils::Method(&cls, &MyClass::MyFunction));

        ApplicationBase::Get<TestApplication>().Run();

        std::ifstream fin;
        fin.open(cLogFileName);
        CHECK_EQ(fin.is_open(), true);
        std::string line;
        std::getline(fin, line);
        std::getline(fin, line);
        std::getline(fin, line);
        CHECK_EQ(rsp::utils::StrUtils::EndsWith(line, "Logged from callback to member function."), true);
    }

}


