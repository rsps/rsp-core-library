/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <doctest.h>
#include <cstring>
#include <graphics/GfxHal.h>
#include <utils/Crc32.h>
#include <utils/StopWatch.h>
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("GfxHal")
{
    constexpr const uint32_t cWidth  = 4;
    constexpr const uint32_t cHeight = 25;
    constexpr const uint32_t cSize   = (cWidth * cHeight);
    constexpr const uint32_t cByteSize   = (cSize * sizeof(uint32_t));

    VideoSurface src;
    VideoSurface dst;

    CHECK_NOTHROW(GfxHal::Get());
    GfxHal& gfx = GfxHal::Get();
    gfx.SetBlendOperation(GfxBlendOperation::Copy);
    gfx.SetColorKey(0);

    CHECK_NOTHROW(gfx.Alloc(src, cWidth, cHeight));
    CHECK_NOTHROW(gfx.Alloc(dst, cWidth, cHeight));

    CHECK_NOTHROW(
    for (uint32_t i = 0 ; i < cSize ; ++i) {
        src.mpPhysAddr.get()[i] = 0xAA000000 + i;
    });

    SUBCASE("Surface") {
        CHECK_EQ(src.mWidth, cWidth);
        CHECK_EQ(src.mHeight, cHeight);
        CHECK_EQ(src.mRowPitch, sizeof(uint32_t) * cWidth);
        CHECK_EQ(src.mRotation, 0);
    }

    SUBCASE("Copy") {
        SUBCASE("Blit") {
            CHECK_NOTHROW(gfx.Blit(dst, src));
            CHECK_MESSAGE(std::memcmp(dst.mpPhysAddr.get(), src.mpPhysAddr.get(), cByteSize) == 0,
                "src\n" << ToHex(src.mpPhysAddr.get(), cSize) << "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));
        }

        SUBCASE("DrawRect") {
            CHECK_NOTHROW(gfx.DrawRect(dst, 0xFFFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 3518719063u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x11111111, Rect(1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2652070992u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x22222222, Rect(0, 1, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2871923838u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x33333333, Rect(-1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2431894754u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x44444444, Rect(0, -1, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1697079512u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x55555555, Rect(-4, -25, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1697079512u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x66666666, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 259604865u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));
    }

        SUBCASE("Fill") {
            CHECK_NOTHROW(gfx.Fill(dst, 0, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1176636684u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0xAAAAAAAA, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1472934187u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0xBBBBBBBB, Rect(-1, -1, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 3186885033u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0xCCCCCCCC, Rect(3, 18, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 522257633u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

        }
    }

    SUBCASE("SourceAlpha") {
        CHECK_NOTHROW(gfx.Fill(src, 0xAAFFFFFF, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(src.mpPhysAddr.get(), cByteSize) == 1438074527u, "src\n" << ToHex(src.mpPhysAddr.get(), cSize));

        CHECK_NOTHROW(gfx.Fill(dst, 0xFF000000, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 4178597885u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

        gfx.SetBlendOperation(GfxBlendOperation::SourceAlpha);
        gfx.SetColorKey(0xAAFFFFFF);

        SUBCASE("Blit") {
            CHECK_NOTHROW(gfx.Blit(dst, src));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2240954413u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));
        }

        SUBCASE("DrawRect") {
            CHECK_NOTHROW(gfx.DrawRect(dst, 0xAAFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1819834801u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x11111111, Rect(1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1885296249u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x66666666, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 4028978613u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));
    }

        SUBCASE("Fill") {
            CHECK_NOTHROW(gfx.Fill(dst, 0xAAFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2240954413u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0x11111111, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 3509047292u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0x66666666, Rect(-1, -1, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 4134027999u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0xCCCCCCCC, Rect(3, 18, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 542745882u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

        }
    }

    SUBCASE("ColorKey") {
        CHECK_NOTHROW(gfx.Fill(src, 0xFFFFFFFF, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(src.mpPhysAddr.get(), cByteSize) == 2307493135u, "src\n" << ToHex(src.mpPhysAddr.get(), cSize));

        CHECK_NOTHROW(gfx.Fill(src, 0xFF555555, Rect(1, 1, 2, 23)));
        CHECK_MESSAGE(Crc32::Calc(src.mpPhysAddr.get(), cByteSize) == 1190816953u, "src\n" << ToHex(src.mpPhysAddr.get(), cSize));

        CHECK_NOTHROW(gfx.Fill(dst, 0xFF000000, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 4178597885u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

        gfx.SetBlendOperation(GfxBlendOperation::ColorKey);
        gfx.SetColorKey(0x22555555);

        SUBCASE("Blit") {
            CHECK_NOTHROW(gfx.Blit(dst, src));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 558079074u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));
        }

        SUBCASE("DrawRect") {
            CHECK_NOTHROW(gfx.DrawRect(dst, 0xFF222222, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2681254777u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x11555555, Rect(1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2681254777u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x66666666, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2050463010u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));
        }

        SUBCASE("Fill") {
            CHECK_NOTHROW(gfx.Fill(dst, 0xFFFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2307493135u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0x11555555, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 2307493135u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0x66666666, Rect(-1, -1, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1645115526u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

            CHECK_NOTHROW(gfx.Fill(dst, 0xCCCCCCCC, Rect(3, 18, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), cByteSize) == 1253784020u, "dst\n" << ToHex(dst.mpPhysAddr.get(), cSize));

        }
    }

    CHECK_NOTHROW(dst.mpPhysAddr = nullptr);
    CHECK_NOTHROW(src.mpPhysAddr = nullptr);

    SUBCASE("Performance") {
        CHECK_NOTHROW(gfx.Alloc(src, 480, 800));
        CHECK_NOTHROW(gfx.Alloc(dst, 480, 800));

        CHECK_NOTHROW(
        for (uint32_t y = 0 ; y < 800 ; ++y) {
            uint32_t line = y * (src.mRowPitch / sizeof(uint32_t));
            for (uint32_t x = 0 ; x < 480 ; ++x) {
                src.mpPhysAddr.get()[line + x] = 0xFF7F00FF;
            }
        });

        SUBCASE("Copy") {
            gfx.SetBlendOperation(GfxBlendOperation::Copy);
            gfx.SetColorKey(0);
        }

        SUBCASE("SourceAlpha") {
            gfx.SetBlendOperation(GfxBlendOperation::SourceAlpha);
            gfx.SetColorKey(0);
        }

        SUBCASE("ColorKey") {
            gfx.SetBlendOperation(GfxBlendOperation::ColorKey);
            gfx.SetColorKey(0);
        }

        StopWatch sw;
        for (int i = 0; i < 1000 ; i++) {
            CHECK_NOTHROW(gfx.Blit(dst, src));
        }
        float mps = ((480 * 800) / (sw.Elapsed<std::chrono::milliseconds>() + 1));
        MESSAGE(mps << " MP/s");
    }

}

TEST_SUITE_END();
