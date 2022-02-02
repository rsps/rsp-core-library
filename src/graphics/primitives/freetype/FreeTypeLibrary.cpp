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
#include <utils/StrUtils.h>

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
    auto it = mFontFileNames.find(arFontName);
    if (it == mFontFileNames.end()) {
        THROW_WITH_BACKTRACE1(FontException,  rsp::utils::StrUtils::Format("Font named %s is not installed.", arFontName.c_str()));
    }

    return it->second.c_str();
}

void FreeTypeLibrary::RegisterFont(const std::string &arFileName)
{
    FT_Face face = nullptr;

    FT_Error error = FT_New_Face(mFtLib, arFileName.c_str(), 0, &face);

    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_New_Face() failed", error);
    }

    mFontFileNames.insert ( std::pair<std::string,std::string>(std::string(face->family_name), arFileName) );
    std::clog << "Font named " << face->family_name << " is installed" << std::endl;

    if (face) {
        FT_Done_Face(face);
    }
/*
    int index = 0;
    do {
        if (face) {
            FT_Done_Face(face);
        }

        FT_Error error = FT_New_Face(mFtLib, arFileName.c_str(), index, &face);

        if (error) {
            THROW_WITH_BACKTRACE2(FontException, "FT_New_Face() failed", error);
        }

        std::cout << "num_faces: " << face->num_faces << "\n"
            << "face_index: " << face->face_index << "\n"
            << "face_flags: " << std::hex << face->face_flags << std::dec << "\n"
            << "style_flags: " << std::hex << face->style_flags << std::dec << "\n"
            << "num_glyphs: " << std::hex << face->num_glyphs << std::dec << "\n"
            << "family_name: " << std::hex << face->family_name << std::dec << "\n"
            << "style_name: " << std::hex << face->style_name << std::dec << "\n"
            << "num_fixed_sizes: " << face->num_fixed_sizes << "\n"
            << "available_sizes: " << face->available_sizes << "\n"
            << "num_charmaps: " << face->num_charmaps << "\n"
            << "charmaps: " << face->charmaps << "\n";
    }
    while(++index < face->num_faces);

    if (face) {
        FT_Done_Face(face);
    }
*/
}

}

