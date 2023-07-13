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
#include <utils/StrUtils.h>
#include <TestHelpers.h>

using namespace rsp::utils;
using namespace rsp::graphics;
using namespace rsp::posix;

TEST_SUITE_BEGIN("Graphics");

static ColorDepth getDepth(std::string aPath)
{
    if (StrUtils::Contains(aPath, "/alpha/")) {
        return ColorDepth::Alpha;
    }
    if (StrUtils::Contains(aPath, "/monochrome/")) {
        return ColorDepth::Monochrome;
    }
    if (StrUtils::Contains(aPath, "/rgb/")) {
        return ColorDepth::RGB;
    }
    return ColorDepth::RGBA;
}

static bool stemCompare(std::filesystem::path a1, std::filesystem::path a2)
{
    return  a1.stem().compare(a2.stem()) < 0;
}


TEST_CASE("Bitmap to C")
{
    TestLogger logger;

    std::string root = FileSystem::GetCurrentWorkingDirectory() + "testImages";
    std::vector<std::filesystem::path> list;

    FileSystem::MakeDirectory(root + "/gfx");
    FileSystem::DeleteFile(root + "/gfx/GfxResources.h");

    if (FileSystem::DirectoryExists(root + "/alpha")) {
        MESSAGE("List alpha");
        auto l = FileSystem::Glob(root + "/alpha/*.bmp");
        list.insert(list.end(), l.begin(), l.end());
    }

    if (FileSystem::DirectoryExists(root + "/monochrome")) {
        MESSAGE("List Monochrome");
        auto l = FileSystem::Glob(root + "/monochrome/*.bmp");
        list.insert(list.end(), l.begin(), l.end());
    }

    if (FileSystem::DirectoryExists(root + "/rgb")) {
        MESSAGE("List RGB");
        auto l = FileSystem::Glob(root + "/rgb/*.bmp");
        list.insert(list.end(), l.begin(), l.end());
    }

    if (FileSystem::DirectoryExists(root + "/rgba")) {
        MESSAGE("List RGBA");
        auto l = FileSystem::Glob(root + "/rgba/*.bmp");
        list.insert(list.end(), l.begin(), l.end());
    }

    std::sort(list.begin(), list.end(), stemCompare);

    for(std::filesystem::path &path : list) {
        MESSAGE(path.stem());
    }

    for(std::filesystem::path &path : list) {
        MESSAGE("Converting " << path.filename() << " to C++ file");
        Bitmap bmp(path);
        PixelData pd = bmp.GetPixelData().ChangeColorDepth(getDepth(path));

        std::string dest = root + "/gfx/" + std::string(path.stem()) + ".cpp";
        pd.SaveToCFile(dest, true, "GfxResources.h");
    }
}


