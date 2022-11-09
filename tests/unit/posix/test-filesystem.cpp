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
#include <locale>
#include <utils/StrUtils.h>
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

        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);

        CHECK(msec.count() >= 100);
    }

    SUBCASE("Get IP Address") {
        std::string ip;

        CHECK_NOTHROW(ip = FileSystem::GetCurrentIpAddress());

        MESSAGE(("Current IP: " + ip));

        CHECK(ip.length() >= 7);
        CHECK(!StrUtils::Contains(ip, "127.0.0.1"));
    }

    SUBCASE("List Directory") {
        std::vector<std::filesystem::path> list = FileSystem::Glob(std::filesystem::path{"/etc/hostn*"});
        CHECK(list[0].string() == "/etc/hostname");
    }

    SUBCASE("Get Framebuffer Device By Driver Name") {
        std::filesystem::path p = FileSystem::GetCharacterDeviceByDriverName("vfb2", std::filesystem::path{"/dev/fb?"});

        MESSAGE((p.empty() ? "Not Found!" : p.string()));

        CHECK(StrUtils::StartsWith(p.string(), "/dev/fb"));
    }

    SUBCASE("File time") {
        using namespace std::literals::chrono_literals;
        using namespace std::chrono;

        std::string filename = "testfile.tmp";

        std::ofstream fout(filename);
        fout << "test" << std::endl;
        fout.close();

        std::string mtime = StrUtils::TimeStamp(FileSystem::GetFileModifiedTime(filename), StrUtils::TimeFormats::HTTP);
        MESSAGE("mtime: " << mtime);

        auto tp = StrUtils::ToTimePoint(mtime, StrUtils::TimeFormats::HTTP);
        mtime = StrUtils::TimeStamp(tp, StrUtils::TimeFormats::HTTP);
        auto d = floor<days>(tp);
        year_month_day ymd(sys_days{d});
        std::chrono::hh_mm_ss<seconds> hms{ floor<seconds>(tp - d)};

//        MESSAGE("mtime2: " << mtime << ", "
//            << static_cast<int>(ymd.year()) << "-" << std::setfill('0')
//            << static_cast<unsigned>(ymd.month()) << "-"
//            << std::setw(1) << static_cast<unsigned>(ymd.day()) << " "
//            << hms.hours().count() << ":"
//            << hms.minutes().count() << ":"
//            << hms.seconds().count()
//            );

        FileSystem::SetFileModifiedTime(filename, tp);

        CHECK_EQ(mtime, StrUtils::TimeStamp(FileSystem::GetFileModifiedTime(filename), StrUtils::TimeFormats::HTTP));

        std::filesystem::remove(filename);
    }
}




