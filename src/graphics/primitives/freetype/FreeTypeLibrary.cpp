/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "FreeTypeLibrary.h"
#include <graphics/primitives/Font.h>

namespace rsp::graphics {

FreeTypeLibrary::FreeTypeLibrary(void)
{
    FT_Error error = FT_Init_FreeType(&mFtLib);
    if (error) {
        THROW_WITH_BACKTRACE3(std::system_error, error, std::generic_category(), "Failed to load library");
    }
}

FreeTypeLibrary::~FreeTypeLibrary(void)
{
    FT_Done_FreeType(mFtLib);
}

FreeTypeLibrary& FreeTypeLibrary::Get()
{
    static FreeTypeLibrary instance;
    return instance;
}

const char* FreeTypeLibrary::GetFileName(const std::string &arFontName) const
{
    return mFontFileNames.at(arFontName).c_str();
}

void FreeTypeLibrary::RegisterFont(const std::string &arFileName)
{
    FT_Face face;

    FT_Error error = FT_New_Face(mFtLib, arFileName.c_str(), 0, &face);

    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_New_Face() failed", error);
    }

    mFontFileNames.insert ( std::pair<std::string,std::string>(std::string(face->family_name), arFileName) );

    if (face) {
        FT_Done_Face(face);
    }
}

}

