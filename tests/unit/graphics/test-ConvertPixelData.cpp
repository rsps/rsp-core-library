/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <doctest.h>
#include <graphics/Bitmap.h>
#include <graphics/Color.h>
#include <posix/FileSystem.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::graphics;
using namespace rsp::posix;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Bitmap to C")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    std::string root = FileSystem::GetCurrentWorkingDirectory() + "testImages";
    ColorDepth depth;
    std::string dir;

    FileSystem::MakeDirectory(root + "/gfx");

    SUBCASE("Alpha") {
        FileSystem::DeleteFile(root + "/gfx/GfxResources.h");
        MESSAGE("Converting alpha");
        dir = root + "/alpha";
        depth = ColorDepth::Alpha;
    }

    SUBCASE("Monochrome") {
        MESSAGE("Converting monochrome");
        dir = root + "/monochrome";
        depth = ColorDepth::Monochrome;
    }

    SUBCASE("RGB") {
        MESSAGE("Converting rgb");
        dir = root + "/rgb";
        depth = ColorDepth::RGB;
    }

    SUBCASE("RGBA") {
        MESSAGE("Converting rgba");
        dir = root + "/rgba";
        depth = ColorDepth::RGBA;
    }

    REQUIRE_MESSAGE(FileSystem::DirectoryExists(dir), "The directory " << dir << " does not exist.");

    std::vector<std::filesystem::path> list = FileSystem::Glob(dir + "/*.bmp");
    for(std::filesystem::path &path : list) {
        MESSAGE("Converting " << path.filename() << " to C++ file");
        Bitmap bmp(path);
        PixelData pd = bmp.GetPixelData().ChangeColorDepth(depth);

        std::string dest = root + "/gfx/" + std::string(path.stem()) + ".cpp";
        pd.SaveToCFile(dest, true, "GfxResources.h");
    }
}


