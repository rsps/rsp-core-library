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
#include <graphics/primitives/Font.h>
#include <graphics/primitives/Rect.h>
#include <graphics/primitives/Text.h>
#include <TestHelpers.h>

using namespace rsp::graphics;

TEST_CASE("Font Primitive")
{
    rsp::logging::Logger logger;
    TestHelpers::AddConsoleLogger(logger);

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
        CHECK(glyphs.size() == 1);
        CHECK(glyphs[0].mHeight > 0);
        CHECK(glyphs[0].mHeight < 16);
        CHECK(glyphs[0].mWidth > 0);
        CHECK(glyphs[0].mWidth < 16);
    }

    SUBCASE("Get Text Mask") {
        CHECK_NOTHROW(Font::RegisterFont(cFontFile));
        Text text(cFontName, "Hello World");
        const int size = 16;

        text.GetFont().SetSize(size);

        auto glyphs = text.GetGlyphs();
        Rect r = text.CalcBoundingRect(glyphs);

        CHECK(r.GetHeight() < size);
        CHECK(r.GetWidth() < (size * text.GetValue().size()));
    }

    SUBCASE("Scale To fit") {
        CHECK_NOTHROW(Font::RegisterFont(cFontFile));
        Text text(cFontName, "Hello World");
        const int size = 16;

        Rect dst(100, 200, 280, 200);
        text.SetScaleToFit(true).SetArea(dst).Reload();

        auto glyphs = text.GetGlyphs();
        Rect r = text.CalcBoundingRect(glyphs);

        MESSAGE(r.GetHeight(), " < ", dst.GetHeight());
        MESSAGE(r.GetWidth(), " < ", dst.GetWidth());
        CHECK(r.GetHeight() < dst.GetHeight());
        CHECK(r.GetWidth() < dst.GetWidth());
    }
}

