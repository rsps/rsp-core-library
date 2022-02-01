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

using namespace rsp::graphics;

TEST_CASE("Font Primitive")
{
    const char* cFontFile = "fonts/Exo2-VariableFont_wght.ttf";

    SUBCASE("Load Library") {
        FreeTypeLibrary lib { };
        FT_Library ft;
        CHECK_NOTHROW(ft = lib); // Call operator
        CHECK(ft);
    }

    SUBCASE("Load Font") {
        Font font(cFontFile);
        FontFace ff;
        CHECK_NOTHROW(ff = font); // call operator
        CHECK(ff);
    }

    SUBCASE("Get Single Symbol Mask") {
        Font font(cFontFile);
        font.SetSize(16);

        Glyph tm = font.GetSymbol('A');
        CHECK(tm.mSymbolUnicode == 'A');
        CHECK(tm.mHeight > 0);
        CHECK(tm.mHeight < 16);
        CHECK(tm.mWidth > 0);
        CHECK(tm.mWidth < 16);
    }

    SUBCASE("Get Text Mask") {
        const std::string text("Hello World");
        const int size = 16;

        Font font(cFontFile);
        font.SetSize(size);

        auto tms = font.MakeTextMasks(text);
        Rect r = font.GetTextBoundingRect(tms);

        CHECK(r.GetHeight() < size);
        CHECK(r.GetWidth() < (size * text.size()));
    }

    SUBCASE("Scale To fit") {
        const std::string text("Hello World");
        Font font(cFontFile);
        Rect dst(100, 200, 280, 200);

        font.ScaleToFit(text, dst.GetWidth(), dst.GetHeight());

        auto tms = font.MakeTextMasks(text);
        Rect r = font.GetTextBoundingRect(tms);

        MESSAGE(r.GetHeight(), " < ", dst.GetHeight());
        MESSAGE(r.GetWidth(), " < ", dst.GetWidth());
        CHECK(r.GetHeight() < dst.GetHeight());
        CHECK(r.GetWidth() < dst.GetWidth());
    }
}

