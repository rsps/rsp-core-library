/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <posix/FileIO.h>
#include <unistd.h>

using namespace rsp::posix;

TEST_CASE("File IO") {
    const std::string cFileName = "temp.txt";

    SUBCASE("Construct") {
        CHECK_NOTHROW(FileIO f);
    }

    SUBCASE("Create File") {
        CHECK_NOTHROW(FileIO f(cFileName, std::ios_base::out, 0664));
    }

    SUBCASE("Write File") {
        FileIO f(cFileName, std::ios_base::out);

        CHECK_NOTHROW(f.PutContents("Hello World"));
    }

    SUBCASE("Read File") {
        FileIO f(cFileName, std::ios_base::in);
        std::string s = f.GetContents();

        CHECK(s == "Hello World");
    }

    SUBCASE("Seek And Read") {
        FileIO f(cFileName, std::ios_base::in);
        f.Seek(6, std::ios_base::beg);
        std::string s = f.GetLine();

        CHECK(s == "World");
    }

    SUBCASE("Seek And Write") {
        FileIO f(cFileName, std::ios_base::out);
        f.Seek(6, std::ios_base::beg);

        CHECK_NOTHROW(f.PutLine("File"));
        CHECK_NOTHROW(f.SetSize(10));
    }

    SUBCASE("Read Hello") {
        FileIO f(cFileName, std::ios_base::in);
        std::string s = f.GetContents();

        CHECK(s == "Hello File");
    }

    SUBCASE("Cleanup") {
        unlink(cFileName.c_str());
    }
}
