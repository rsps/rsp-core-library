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
#include <rsp/graphics/PixelData.h>
#include <TestHelpers.h>
#include <rsp/posix/FileSystem.h>

using namespace rsp::graphics;

const uint8_t cImageRGBA[] = {
    0xFF, 0xFF, 0xFF, 0xFF,  0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0xFF,  0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x80,  0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x80,  0xFF, 0xFF, 0xFF, 0x80,
};

const uint8_t cImageRGB[] = {
    0xFF, 0xFF, 0xFF,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0xFF, 0xFF, 0xFF
};

const uint8_t cImageAlpha[] = {
    0x10, 0x20, 0x30, 0x40,  0x50, 0x60, 0x70, 0x80,  0x90, 0xA0, 0xB0, 0xC0,  0xD0, 0xE0, 0xF0, 0xFF,
    0x10, 0x20, 0x30, 0x40,  0x50, 0x60, 0x70, 0x80,  0x90, 0xA0, 0xB0, 0xC0,  0xD0, 0xE0, 0xF0, 0xFF
};

const uint8_t cImage1bit[] = {
    0xAA, 0xAA,
    0x55, 0x55,
    0x33, 0x22,
    0x11, 0xFF
};

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("PixelData")
{
    TestLogger logger;

    SUBCASE("Constructs")
    {
        CHECK_NOTHROW(PixelData tmp1);
        CHECK_NOTHROW(PixelData tmp2(10, 10, ColorDepth::Monochrome));
        CHECK_NOTHROW(PixelData tmp3(10, 10, ColorDepth::Alpha));
        CHECK_NOTHROW(PixelData tmp4(10, 10, ColorDepth::RGB));
        CHECK_NOTHROW(PixelData tmp5(2, 2, ColorDepth::RGB, cImageRGB, sizeof(cImageRGB)));
        CHECK_NOTHROW(PixelData tmp6(8, 8, ColorDepth::Monochrome, cImage1bit, sizeof(cImage1bit)));
    }

    SUBCASE("Monochrome")
    {
        PixelData pd(8, 8, ColorDepth::Monochrome, cImage1bit, sizeof(cImage1bit));
        CHECK_EQ(pd.GetWidth(), 8);
        CHECK_EQ(pd.GetHeight(), 8);
        CHECK_EQ(pd.GetColorDepth(), ColorDepth::Monochrome);

        CHECK_HEX(pd.GetPixelAt(0,0, Color::White).ToARGB(), 0x00FFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(7,7, Color::White).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(8,7, Color::White).ToARGB(), Color::None);
        CHECK_EQ(pd.GetDataSize(), 8u);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::White));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White).ToARGB(), 0xFFFFFFFF);
        CHECK_EQ(pd.GetDataSize(), 8u);

        rsp::posix::FileSystem::MakeDirectory("cfiles");
        pd.SaveToCFile(std::string("cfiles/Monochrome.cpp"));
    }

    SUBCASE("Alpha")
    {
        PixelData pd(8, 4, ColorDepth::Alpha, cImageAlpha, sizeof(cImageAlpha));
        CHECK_EQ(pd.GetWidth(), 8);
        CHECK_EQ(pd.GetHeight(), 4);
        CHECK_EQ(pd.GetColorDepth(), ColorDepth::Alpha);

        CHECK_EQ(pd.GetPixelAt(0,0, Color::White).ToARGB(), 0x10FFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0x20FFFFFFu);
        CHECK_EQ(pd.GetPixelAt(7,3, Color::White).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 32u);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::White));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 32u);

        pd.SaveToCFile(std::string("cfiles/Alpha.cpp"));
    }

    SUBCASE("RGB")
    {
        PixelData pd(2, 2, ColorDepth::RGB, cImageRGB, sizeof(cImageRGB));
        CHECK_EQ(pd.GetWidth(), 2);
        CHECK_EQ(pd.GetHeight(), 2);
        CHECK_EQ(pd.GetColorDepth(), ColorDepth::RGB);

        CHECK_EQ(pd.GetPixelAt(0,0, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(0,1, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(1,1, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 12u);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::Blue));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White).ToARGB(), Color::Blue);
        CHECK_EQ(pd.GetDataSize(), 12u);

        pd.SaveToCFile(std::string("cfiles/RGB.cpp"));
    }

    SUBCASE("RGBA")
    {
        PixelData pd(2, 4, ColorDepth::RGBA, cImageRGBA, sizeof(cImageRGBA));
        CHECK_EQ(pd.GetWidth(), 2);
        CHECK_EQ(pd.GetHeight(), 4);
        CHECK_EQ(pd.GetColorDepth(), ColorDepth::RGBA);

        CHECK_EQ(pd.GetPixelAt(0,0, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(0,1, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(1,1, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetPixelAt(0,2, Color::Black).ToARGB(), 0x80FFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,2, Color::White).ToARGB(), 0x80000000u);
        CHECK_EQ(pd.GetPixelAt(0,3, Color::White).ToARGB(), 0x80000000u);
        CHECK_EQ(pd.GetPixelAt(1,3, Color::Black).ToARGB(), 0x80FFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 32u);

        CHECK_NOTHROW(pd.SetPixelAt(0,0, Color::Blue));
        CHECK_EQ(pd.GetPixelAt(0,0, Color::White).ToARGB(), Color::Blue);
        CHECK_EQ(pd.GetDataSize(), 32u);

        pd.SaveToCFile(std::string("cfiles/RGB.cpp"));
    }

    SUBCASE("Compress Monochrome")
    {
        PixelData in(8, 8, ColorDepth::Monochrome, cImage1bit, sizeof(cImage1bit));

        GfxCompressor::CompressedData cd;
        CHECK_NOTHROW(cd = in.Compress());

        CHECK_EQ(cd.mData.size(), 12u);
//        MESSAGE("Compressed:\n" << TestHelpers::ToHex(cd.mData.data(), cd.mData.size(), 1));

        PixelData pd(8, 8, ColorDepth::Monochrome);
        CHECK_NOTHROW(pd.Decompress(cd));
//        MESSAGE("Decompressed:\n" << TestHelpers::ToHex(pd.GetData().data(), pd.GetDataSize(), 1));

        CHECK_HEX(pd.GetPixelAt(0,0, Color::White).ToARGB(), 0x00FFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(7,7, Color::White).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(8,7, Color::White).ToARGB(), Color::None);
        CHECK_EQ(pd.GetDataSize(), 8u);
    }

    SUBCASE("Compress Alpha")
    {
        PixelData in(8, 4, ColorDepth::Alpha, cImageAlpha, sizeof(cImageAlpha));

        GfxCompressor::CompressedData cd;
        CHECK_NOTHROW(cd = in.Compress());

        CHECK_EQ(cd.mData.size(), 64u);
//        MESSAGE("Compressed:\n" << TestHelpers::ToHex(cd.mData.data(), cd.mData.size(), 1));

        PixelData pd(8, 4, ColorDepth::Alpha);
        CHECK_NOTHROW(pd.Decompress(cd));
//        MESSAGE("Decompressed:\n" << TestHelpers::ToHex(pd.GetData().data(), pd.GetDataSize(), 1));

        CHECK_EQ(pd.GetPixelAt(0,0, Color::White).ToARGB(), 0x10FFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0x20FFFFFFu);
        CHECK_EQ(pd.GetPixelAt(7,3, Color::White).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 32u);
    }

    SUBCASE("Compress RGB")
    {
        PixelData in(2, 2, ColorDepth::RGB, cImageRGB, sizeof(cImageRGB));

        GfxCompressor::CompressedData cd;
        CHECK_NOTHROW(cd = in.Compress());

        CHECK_EQ(cd.mData.size(), 12u);
//        MESSAGE("Compressed:\n" << TestHelpers::ToHex(cd.mData.data(), cd.mData.size(), 1));

        PixelData pd(2, 2, ColorDepth::RGB);
        CHECK_NOTHROW(pd.Decompress(cd));
//        MESSAGE("Decompressed:\n" << TestHelpers::ToHex(pd.GetData().data(), pd.GetDataSize(), 1));

        CHECK_EQ(pd.GetPixelAt(0,0, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(0,1, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(1,1, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 12u);

    }

    SUBCASE("Compress RGBA")
    {
        PixelData in(2, 4, ColorDepth::RGBA, cImageRGBA, sizeof(cImageRGBA));

        GfxCompressor::CompressedData cd;
        CHECK_NOTHROW(cd = in.Compress());

        CHECK_EQ(cd.mData.size(), 30u);
//        MESSAGE("Compressed:\n" << TestHelpers::ToHex(cd.mData.data(), cd.mData.size(), 1));

        PixelData pd(2, 4, ColorDepth::RGBA);
        CHECK_NOTHROW(pd.Decompress(cd));
//        MESSAGE("Decompressed:\n" << TestHelpers::ToHex(pd.GetData().data(), pd.GetDataSize(), 1));

        CHECK_EQ(pd.GetPixelAt(0,0, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,0, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(0,1, Color::White).ToARGB(), 0xFF000000u);
        CHECK_EQ(pd.GetPixelAt(1,1, Color::Black).ToARGB(), 0xFFFFFFFFu);
        CHECK_EQ(pd.GetPixelAt(0,2, Color::Black).ToARGB(), 0x80FFFFFFu);
        CHECK_EQ(pd.GetPixelAt(1,2, Color::White).ToARGB(), 0x80000000u);
        CHECK_EQ(pd.GetPixelAt(0,3, Color::White).ToARGB(), 0x80000000u);
        CHECK_EQ(pd.GetPixelAt(1,3, Color::Black).ToARGB(), 0x80FFFFFFu);
        CHECK_EQ(pd.GetDataSize(), 32u);
    }

    SUBCASE("Fade Out")
    {
        PixelData pd(2, 4, ColorDepth::RGBA, cImageRGBA, sizeof(cImageRGBA));

        pd.Fade(-64);
        CHECK_HEX(pd.GetPixelAt(0,0).ToARGB(), 0xBFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0).ToARGB(), 0xBF000000);
        CHECK_HEX(pd.GetPixelAt(0,1).ToARGB(), 0xBF000000);
        CHECK_HEX(pd.GetPixelAt(1,1).ToARGB(), 0xBFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(0,2).ToARGB(), 0x40FFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,2).ToARGB(), 0x40000000);
        CHECK_HEX(pd.GetPixelAt(0,3).ToARGB(), 0x40000000);
        CHECK_HEX(pd.GetPixelAt(1,3).ToARGB(), 0x40FFFFFF);

        pd.Fade(-64);
        CHECK_HEX(pd.GetPixelAt(0,0).ToARGB(), 0x7FFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0).ToARGB(), 0x7F000000);
        CHECK_HEX(pd.GetPixelAt(0,1).ToARGB(), 0x7F000000);
        CHECK_HEX(pd.GetPixelAt(1,1).ToARGB(), 0x7FFFFFFF);
        CHECK_HEX(pd.GetPixelAt(0,2).ToARGB(), 0x00FFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,2).ToARGB(), 0x00000000);
        CHECK_HEX(pd.GetPixelAt(0,3).ToARGB(), 0x00000000);
        CHECK_HEX(pd.GetPixelAt(1,3).ToARGB(), 0x00FFFFFF);

        pd.Fade(-64);
        CHECK_HEX(pd.GetPixelAt(0,0).ToARGB(), 0x3FFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0).ToARGB(), 0x3F000000);
        CHECK_HEX(pd.GetPixelAt(0,1).ToARGB(), 0x3F000000);
        CHECK_HEX(pd.GetPixelAt(1,1).ToARGB(), 0x3FFFFFFF);
        CHECK_HEX(pd.GetPixelAt(0,2).ToARGB(), 0x00FFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,2).ToARGB(), 0x00000000);
        CHECK_HEX(pd.GetPixelAt(0,3).ToARGB(), 0x00000000);
        CHECK_HEX(pd.GetPixelAt(1,3).ToARGB(), 0x00FFFFFF);
    }

    SUBCASE("Fade In")
    {
        PixelData pd(2, 4, ColorDepth::RGBA, cImageRGBA, sizeof(cImageRGBA));

        pd.Fade(64);
        CHECK_HEX(pd.GetPixelAt(0,0).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(0,1).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(1,1).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(0,2).ToARGB(), 0xC0FFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,2).ToARGB(), 0xC0000000);
        CHECK_HEX(pd.GetPixelAt(0,3).ToARGB(), 0xC0000000);
        CHECK_HEX(pd.GetPixelAt(1,3).ToARGB(), 0xC0FFFFFF);

        pd.Fade(64);
        CHECK_HEX(pd.GetPixelAt(0,0).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(0,1).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(1,1).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(0,2).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,2).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(0,3).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(1,3).ToARGB(), 0xFFFFFFFF);

        pd.Fade(64);
        CHECK_HEX(pd.GetPixelAt(0,0).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,0).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(0,1).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(1,1).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(0,2).ToARGB(), 0xFFFFFFFF);
        CHECK_HEX(pd.GetPixelAt(1,2).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(0,3).ToARGB(), 0xFF000000);
        CHECK_HEX(pd.GetPixelAt(1,3).ToARGB(), 0xFFFFFFFF);
    }
}

TEST_SUITE_END();

