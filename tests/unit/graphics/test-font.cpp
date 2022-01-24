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

        TextMask tm = font.GetSymbol('A');
        CHECK(tm.mSymbolUnicode == 'A');
        CHECK(tm.mHeight > 0);
        CHECK(tm.mHeight < 16);
        CHECK(tm.mWidth > 0);
        CHECK(tm.mWidth < 16);
    }

    SUBCASE("Get Text Mask") {
        Font font(cFontFile);
        font.SetSize(16);

        const std::string text("Hello World");

        TextMask tm = font.MakeTextMask(text);
        CHECK(tm.mHeight < 16);
        CHECK(tm.mWidth < (16 * text.size()));
    }
}

