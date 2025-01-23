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
#include <posix/FileSystem.h>
#include <string>
#include <fstream>
#include <chrono>
#include <utils/StrUtils.h>
#include <utils/DateTime.h>
#include <algorithm>
#include <sstream>

using namespace rsp::posix;
using namespace rsp::utils;
using namespace std::chrono;

TEST_CASE("FileSystem") {

    const std::string cSubDir = "subdir/subdir1/subdir2";
    std::string cwd;

    SUBCASE("Directory Exists") {
        CHECK_NOTHROW( (cwd = FileSystem::GetCurrentWorkingDirectory()) );

        CHECK(FileSystem::DirectoryExists(cwd));
    }

    SUBCASE("Make Directory") {
        CHECK_NOTHROW(FileSystem::MakeDirectory(cwd + cSubDir));
    }

    SUBCASE("Set Permissions") {
        std::string filename = cwd + cSubDir + "/tmpfile";

        std::ofstream fout(filename);
        fout << "test" << std::endl;
        fout.close();

        CHECK_NOTHROW(FileSystem::SetPermissions(filename, 0400));

        std::ofstream fo;
        fo.exceptions(std::ios_base::failbit | std::ios_base::badbit);
        CHECK_THROWS_AS(fo.open(filename), std::ios_base::failure);
    }

    SUBCASE("Make Symlink") {
        CHECK_NOTHROW(FileSystem::MakeSymlink(cwd + cSubDir, cwd + "subdir-link"));

        CHECK(FileSystem::DirectoryExists(cwd + "subdir-link"));
    }

    SUBCASE("Recursive Delete Directory") {
        CHECK_NOTHROW(FileSystem::RecursiveDeleteDir(cwd + "subdir"));
        CHECK_NOTHROW(FileSystem::RecursiveDeleteDir(cwd + "subdir-link"));
    }

    SUBCASE("Sleep") {
        auto start = std::chrono::system_clock::now();

        CHECK_NOTHROW(FileSystem::Sleep(100));

        auto milli_sec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

        CHECK_GE(milli_sec.count(), 100);
    }

    SUBCASE("Get IP Address") {
        std::string ip;

        CHECK_NOTHROW(ip = FileSystem::GetCurrentIpAddress());

        MESSAGE(("Current IP: " + ip));

        CHECK_GE(ip.length(), 7);
        CHECK(!StrUtils::Contains(ip, "127.0.0.1"));
    }

    SUBCASE("List Directory") {
        std::vector<std::filesystem::path> list = FileSystem::Glob(std::filesystem::path{"/etc/hostn*"});
        CHECK(list[0].string() == "/etc/hostname");
    }

    SUBCASE("Get Framebuffer Device By Driver Name") {
        std::filesystem::path p = FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});

        MESSAGE((p.empty() ? "Not Found!" : p.string()));

        if (!p.empty()) {
            CHECK(StrUtils::StartsWith(p.string(), "/dev/fb"));
        }
    }

    SUBCASE("File time") {
        using namespace std::literals::chrono_literals;
        using namespace std::chrono;

        std::string filename = "testfile.tmp";

        std::ofstream fout(filename);
        fout << "test" << std::endl;
        fout.close();

        DateTime mfdt(FileSystem::GetFileModifiedTime(filename));
        MESSAGE("mtime: " << mfdt.ToHTTP());

        mfdt -= std::chrono::seconds(5);

        FileSystem::SetFileModifiedTime(filename, mfdt);

        CHECK_EQ(mfdt.ToRFC3339Milli(), FileSystem::GetFileModifiedTime(filename).ToRFC3339Milli());

        std::filesystem::remove(filename);
    }
}




