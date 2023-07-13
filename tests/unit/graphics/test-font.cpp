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
#include <graphics/Font.h>
#include <graphics/Rect.h>
#include <graphics/Text.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

TEST_SUITE_BEGIN("Graphics");

TEST_CASE("Font")
{
    TestLogger logger;

    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";
    const char* cFontName = "Exo 2";

    SUBCASE("Load Font") {
        CHECK_NOTHROW(Font::RegisterFont(cFontFile));
    }

    SUBCASE("Get Single Symbol Mask") {
        CHECK_NOTHROW(Font::RegisterFont(cFontFile));
        Font font(cFontName);
        font.SetSize(16);

        auto glyphs = font.MakeGlyphs("A");
        CHECK(glyphs->GetCount() == 1);
        CHECK(glyphs->GetGlyph(0).mHeight > 0);
        CHECK(glyphs->GetGlyph(0).mHeight < 16);
        CHECK(glyphs->GetGlyph(0).mWidth > 0);
        CHECK(glyphs->GetGlyph(0).mWidth < 16);
    }

    SUBCASE("Get Text Mask") {
        CHECK_NOTHROW(Font::RegisterFont(cFontFile));
        Text text(cFontName, "qwertyuiopasdfghjklzxcvbnm1234567890-_:;()&@\".,?!'/* []{}#%+~ =¤\\=<>£€$.,?!'/*");
        const int size = 16;

        text.GetFont().SetSize(size);

        Rect r = text.Reload().GetBoundingRect();

        CHECK(r.GetHeight() < size+4);
        CHECK(r.GetWidth() < (size * text.GetValue().size()));
    }

    SUBCASE("Scale To fit") {
        CHECK_NOTHROW(Font::RegisterFont(cFontFile));
        Text text(cFontName, "Hello World");
        const int size = 16;

        Rect dst(100, 200, 280, 200);
        text.Reload(dst);

        Rect r = text.GetBoundingRect();

        MESSAGE(r.GetHeight(), " < ", dst.GetHeight());
        MESSAGE(r.GetWidth(), " < ", dst.GetWidth());
        CHECK(r.GetHeight() < dst.GetHeight());
        CHECK(r.GetWidth() < dst.GetWidth());
    }
}

TEST_SUITE_END();
