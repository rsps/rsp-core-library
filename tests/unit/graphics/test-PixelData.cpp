/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <graphics/primitives/PixelData.h>
#include <TestHelpers.h>
#include <posix/FileSystem.h>

using namespace rsp::graphics;

const std::uint8_t cImageRGB[] = {
    0xFF, 0xFF, 0xFF,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0xFF, 0xFF, 0xFF
};

const std::uint8_t cImageAlpha[] = {
    0x10, 0x20, 0x30, 0x40,  0x50, 0x60, 0x70, 0x80,  0x90, 0xA0, 0xB0, 0xC0,  0xD0, 0xE0, 0xF0, 0xFF,
    0x10, 0x20, 0x30, 0x40,  0x50, 0x60, 0x70, 0x80,  0x90, 0xA0, 0xB0, 0xC0,  0xD0, 0xE0, 0xF0, 0xFF
};

const std::uint8_t cImage1bit[] = {
    0xAA, 0xAA,
    0x55, 0x55,
    0x33, 0x22,
    0x11, 0xFF
};

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("PixelData")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

    SUBCASE("Constructs")
    {
        CHECK_NOTHROW(PixelData tmp1);
        CHECK_NOTHROW(PixelData tmp2(10, 10, PixelData::ColorDepth::Monochrome));
        CHECK_NOTHROW(PixelData tmp3(10, 10, PixelData::ColorDepth::Alpha));
        CHECK_NOTHROW(PixelData tmp4(10, 10, PixelData::ColorDepth::RGB));
        CHECK_NOTHROW(PixelData tmp5(2, 2, PixelData::ColorDepth::RGB, cImageRGB));
        CHECK_NOTHROW(PixelData tmp6(8, 8, PixelData::ColorDepth::Monochrome, cImage1bit));
    }

    SUBCASE("Monochrome")
    {
        PixelData pd(8, 8, PixelData::ColorDepth::Monochrome, cImage1bit);
        CHECK_EQ(pd.GetWidth(), 8);
        CHECK_EQ(pd.GetHeight(), 8);
        CHECK_EQ(pd.GetColorDepth(), PixelData::ColorDepth::Monochrome);

        CHECK_EQ(pd.GetPixelAt(0,0, Color::White), Color(0x00FFFFFF));
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White), Color(0xFFFFFFFF));
        CHECK_EQ(pd.GetPixelAt(7,7, Color::White), Color(0xFFFFFFFF));
        CHECK_THROWS_AS(pd.GetPixelAt(8,7, Color::White), std::exception);
        CHECK_EQ(pd.GetDataSize(), 8);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::White));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White), Color(0xFFFFFFFF));
        CHECK_EQ(pd.GetDataSize(), 8);

        rsp::posix::FileSystem::MakeDirectory("cfiles");
        pd.SaveToCFile(std::string("cfiles/Monochrome.cpp"));
    }

    SUBCASE("Alpha")
    {
        PixelData pd(8, 4, PixelData::ColorDepth::Alpha, cImageAlpha);
        CHECK_EQ(pd.GetWidth(), 8);
        CHECK_EQ(pd.GetHeight(), 4);
        CHECK_EQ(pd.GetColorDepth(), PixelData::ColorDepth::Alpha);

        CHECK_EQ(pd.GetPixelAt(0,0, Color::White), Color(0x10FFFFFF));
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White), Color(0x20FFFFFF));
        CHECK_EQ(pd.GetPixelAt(7,3, Color::White), Color(0xFFFFFFFF));
        CHECK_EQ(pd.GetDataSize(), 32);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::White));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White), Color(0xFFFFFFFF));
        CHECK_EQ(pd.GetDataSize(), 32);

        pd.SaveToCFile(std::string("cfiles/Alpha.cpp"));
    }

    SUBCASE("RGB")
    {
        PixelData pd(2, 2, PixelData::ColorDepth::RGB, cImageRGB);
        CHECK_EQ(pd.GetWidth(), 2);
        CHECK_EQ(pd.GetHeight(), 2);
        CHECK_EQ(pd.GetColorDepth(), PixelData::ColorDepth::RGB);

        CHECK_EQ(pd.GetPixelAt(0,0, Color::Black), Color(0xFFFFFFFF));
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White), Color(0xFF000000));
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White), Color(0xFF000000));
        CHECK_EQ(pd.GetPixelAt(1,1, Color::Black), Color(0xFFFFFFFF));
        CHECK_EQ(pd.GetDataSize(), 12);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::Blue));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White), Color::Blue);
        CHECK_EQ(pd.GetDataSize(), 12);

        pd.SaveToCFile(std::string("cfiles/RGB.cpp"));
    }

}

TEST_SUITE_END();

