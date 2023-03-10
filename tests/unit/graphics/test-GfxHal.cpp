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
#include <TestHelpers.h>

using namespace rsp::graphics;
using namespace rsp::utils;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("GfxHal")
{
    VideoSurface src;
    VideoSurface dst;

    CHECK_NOTHROW(GfxHal::Get());
    GfxHal& gfx = GfxHal::Get();
    gfx.SetBlendOperation(GfxBlendOperation::Copy);
    gfx.SetColorKey(0);

    CHECK_NOTHROW(gfx.Alloc(src, 4, 25));
    CHECK_NOTHROW(gfx.Alloc(dst, 4, 25));

    CHECK_NOTHROW(
    for (int i = 0 ; i < 100 ; ++i) {
        src.mpPhysAddr.get()[i] = 0xAA000000 + uint32_t(i);
    });

    SUBCASE("Surface") {
        CHECK_EQ(src.mWidth, 4);
        CHECK_EQ(src.mHeight, 25);
        CHECK_EQ(src.mRowPitch, 16);
        CHECK_EQ(src.mRotation, 0);
    }

    SUBCASE("Copy") {
        SUBCASE("Blit") {
            CHECK_NOTHROW(gfx.Blit(dst, src));
            CHECK_MESSAGE(std::memcmp(dst.mpPhysAddr.get(), src.mpPhysAddr.get(), 100*sizeof(uint32_t)) == 0,
                "src\n" << ToHex(src.mpPhysAddr.get(), 100) << "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
        }

        SUBCASE("DrawRect") {
            CHECK_NOTHROW(gfx.DrawRect(dst, 0xFFFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 3518719063u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x11111111, Rect(1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2652070992u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x22222222, Rect(0, 1, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2871923838u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x33333333, Rect(-1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2431894754u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x44444444, Rect(0, -1, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1697079512u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x55555555, Rect(-4, -25, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1697079512u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x66666666, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 259604865u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
    }

        SUBCASE("Fill") {
            CHECK_NOTHROW(gfx.Fill(dst, 0, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1176636684u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0xAAAAAAAA, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1472934187u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0xBBBBBBBB, Rect(-1, -1, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 3186885033u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0xCCCCCCCC, Rect(3, 18, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 522257633u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

        }
    }

    SUBCASE("SourceAlpha") {
        CHECK_NOTHROW(gfx.Fill(src, 0xAAFFFFFF, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(src.mpPhysAddr.get(), 400) == 1438074527u, "src\n" << ToHex(src.mpPhysAddr.get(), 100));

        CHECK_NOTHROW(gfx.Fill(dst, 0xFF000000, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 4178597885u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

        gfx.SetBlendOperation(GfxBlendOperation::SourceAlpha);
        gfx.SetColorKey(0xAAFFFFFF);

        SUBCASE("Blit") {
            CHECK_NOTHROW(gfx.Blit(dst, src));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2240954413u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
        }

        SUBCASE("DrawRect") {
            CHECK_NOTHROW(gfx.DrawRect(dst, 0xAAFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1819834801u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x11111111, Rect(1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1885296249u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x66666666, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 4028978613u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
    }

        SUBCASE("Fill") {
            CHECK_NOTHROW(gfx.Fill(dst, 0xAAFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2240954413u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0x11111111, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 3509047292u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0x66666666, Rect(-1, -1, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 4134027999u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0xCCCCCCCC, Rect(3, 18, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 542745882u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

        }
    }

    SUBCASE("ColorKey") {
        CHECK_NOTHROW(gfx.Fill(src, 0xFFFFFFFF, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(src.mpPhysAddr.get(), 400) == 2307493135u, "src\n" << ToHex(src.mpPhysAddr.get(), 100));

        CHECK_NOTHROW(gfx.Fill(src, 0xFF555555, Rect(1, 1, 2, 23)));
        CHECK_MESSAGE(Crc32::Calc(src.mpPhysAddr.get(), 400) == 1190816953u, "src\n" << ToHex(src.mpPhysAddr.get(), 100));

        CHECK_NOTHROW(gfx.Fill(dst, 0xFF000000, Rect(0, 0, 4, 25)));
        CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 4178597885u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

        gfx.SetBlendOperation(GfxBlendOperation::AlphaKey);
        gfx.SetColorKey(0x22555555);

        SUBCASE("Blit") {
            CHECK_NOTHROW(gfx.Blit(dst, src));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 558079074u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
        }

        SUBCASE("DrawRect") {
            CHECK_NOTHROW(gfx.DrawRect(dst, 0xFF222222, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2681254777u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x11555555, Rect(1, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2681254777u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.DrawRect(dst, 0x66666666, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2050463010u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
    }

        SUBCASE("Fill") {
            CHECK_NOTHROW(gfx.Fill(dst, 0xFFFFFFFF, Rect(0, 0, 4, 25)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2307493135u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0x11555555, Rect(1, 1, 2, 23)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 2307493135u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0x66666666, Rect(-1, -1, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1645115526u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

            CHECK_NOTHROW(gfx.Fill(dst, 0xCCCCCCCC, Rect(3, 18, 2, 8)));
            CHECK_MESSAGE(Crc32::Calc(dst.mpPhysAddr.get(), 400) == 1253784020u, "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

        }
    }

    CHECK_NOTHROW(dst.mpPhysAddr = nullptr);
    CHECK_NOTHROW(src.mpPhysAddr = nullptr);
}

TEST_SUITE_END();
