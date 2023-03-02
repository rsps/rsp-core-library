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

    SUBCASE("Alpha") {
        MESSAGE("Converting alphas");
        if (!FileSystem::DirectoryExists(root + "/alpha")) {
            return;
        }
        std::vector<std::filesystem::path> list = FileSystem::Glob(root + "/alpha/*.bmp");

        for(std::filesystem::path &path : list) {
            MESSAGE("Converting " << path << " to C++ file");
            Bitmap bmp(path);
            PixelData alpha = bmp.GetPixelData().ChangeColorDepth(PixelData::ColorDepth::Alpha);
            alpha.SaveToCFile(path.replace_extension("cpp"));
        }
    }

    SUBCASE("Monochrome") {
        if (!FileSystem::DirectoryExists(root + "/monochrome")) {
            return;
        }
        std::vector<std::filesystem::path> list = FileSystem::Glob(root + "/monochrome/*.bmp");

        for(std::filesystem::path &path : list) {
            Bitmap bmp(path);
            PixelData mono = bmp.GetPixelData().ChangeColorDepth(PixelData::ColorDepth::Monochrome);
            mono.SaveToCFile(path.replace_extension("cpp"));
        }
    }

    SUBCASE("RGB") {
        if (!FileSystem::DirectoryExists(root + "/rgb")) {
            return;
        }
        MESSAGE("Found directory " << root + "/rgb");
        std::vector<std::filesystem::path> list = FileSystem::Glob(root+ "/rgb/*.bmp");

        for(std::filesystem::path &path : list) {
            MESSAGE("Convert file " << path);
            Bitmap bmp(path);
            PixelData rgb = bmp.GetPixelData().ChangeColorDepth(PixelData::ColorDepth::RGB);
            rgb.SaveToCFile(path.replace_extension("cpp"));
        }
    }

    SUBCASE("RGBA") {
        if (!FileSystem::DirectoryExists(root + "/rgba")) {
            return;
        }
        std::vector<std::filesystem::path> list = FileSystem::Glob(root + "/rgba/*.bmp");

        for(std::filesystem::path &path : list) {
            Bitmap bmp(path);
            PixelData rgba = bmp.GetPixelData().ChangeColorDepth(PixelData::ColorDepth::RGBA);
            rgba.SaveToCFile(path.replace_extension("cpp"));
        }
    }

}


