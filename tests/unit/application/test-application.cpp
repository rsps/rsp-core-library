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
#include "../../TestApplication.h"

using namespace rsp::application;

const char *cLogFileName = "MyApplication.log";

TEST_CASE("Application Instance Tests")
{
    std::remove(cLogFileName);

    char *arguments[] = {
        "./MyApplication",
        "-c",
        "--help"
    };

    CommandLine cmd(3, arguments);

    TestApplication app(cmd);

    app.showHelp();

    CHECK(std::filesystem::exists(std::filesystem::path(cLogFileName)));
}


