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
#include <TestHelpers.h>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("GfxHal")
{
    VideoSurface src;
    VideoSurface dst;

    CHECK_NOTHROW(GfxHal::Get());
    GfxHal& gfx = GfxHal::Get();

    CHECK_NOTHROW(gfx.Alloc(src, 10, 10));
    CHECK_NOTHROW(gfx.Alloc(dst, 10, 10));

    CHECK_NOTHROW(
    for (int i = 0 ; i < 100 ; ++i) {
        src.mpPhysAddr.get()[i] = uint32_t(i);
    });

    SUBCASE("Surface") {
        CHECK_EQ(src.mWidth, 10);
        CHECK_EQ(src.mHeight, 10);
        CHECK_EQ(src.mRowPitch, 40);
        CHECK_EQ(src.mRotation, 0);
    }

    SUBCASE("Blit") {
        CHECK_NOTHROW(gfx.Blit(dst, src));
        CHECK_MESSAGE(std::memcmp(dst.mpPhysAddr.get(), src.mpPhysAddr.get(), 100*sizeof(uint32_t)) == 0,
            "src\n" << ToHex(src.mpPhysAddr.get(), 100) << "dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
        MESSAGE("dst\n" << ToHex(dst.mpPhysAddr.get(), 100));
    }

    SUBCASE("DrawRect") {
        MESSAGE("dst\n" << ToHex(reinterpret_cast<const uint8_t*>(dst.mpPhysAddr.get()), 400));
        CHECK_NOTHROW(gfx.DrawRect(dst, Rect(0, 0, 10, 10), 0x12345678));
        MESSAGE("dst\n" << ToHex(dst.mpPhysAddr.get(), 100));

        for (uint32_t i = 0 ; i < 10 ; ++i) {
            CHECK_EQ(dst.mpPhysAddr.get()[i], 0x12345678u);
            CHECK_EQ(dst.mpPhysAddr.get()[(10*9) + i], 0x12345678u);
        }
    }

    CHECK_NOTHROW(dst.mpPhysAddr = nullptr);
    CHECK_NOTHROW(src.mpPhysAddr = nullptr);
}

TEST_SUITE_END();
