/*
 * test-filesystem.cpp
 *
 *  Created on: 12. jun. 2020
 *      Author: steffen
 */

#include <doctest.h>
#include <string>
#include <fstream>
#include <utils/StrUtils.h>
#include <filesystem/FileSystem.h>

using namespace rsp;

TEST_CASE("Testing FileSystem") {

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
}




