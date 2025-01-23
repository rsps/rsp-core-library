/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef USE_FREETYPE

#include "FreeTypeLibrary.h"
#include <utils/StrUtils.h>
#include <graphics/FontRawInterface.h>

using namespace rsp::utils;
using namespace rsp::logging;

//#define DEBUG_FONTS 1

namespace rsp::graphics {

void FontRawInterface::RegisterFont(std::string_view aFileName)
{
    FreeTypeLibrary::Get().RegisterFont(std::string(aFileName));
}

FreeTypeLibrary::FreeTypeLibrary()
{
    FT_Error error = FT_Init_FreeType(&mFtLib);
    if (error) {
        THROW_WITH_BACKTRACE3(std::system_error, error, std::generic_category(), "Failed to load library");
    }
}

FreeTypeLibrary::~FreeTypeLibrary()
{
    FT_Done_FreeType(mFtLib);
}

FreeTypeLibrary& FreeTypeLibrary::Get()
{
    static FreeTypeLibrary instance;
    return instance;
}

FT_Face FreeTypeLibrary::CreateFontFace(const std::string &arFontName, FontStyles aStyle)
{
    mLogger.Info() << "Creating font " << arFontName << " with style " << static_cast<int>(aStyle);

    auto it = mFontSets.find(arFontName);
    if (it == mFontSets.end()) {
        THROW_WITH_BACKTRACE1(FontException, StrUtils::Format("Font named %s is not installed.", arFontName.c_str()));
    }

    if (aStyle != FontStyles::Normal) {
        auto inner = mFontSets[arFontName].find(aStyle);
        if (inner == mFontSets[arFontName].end()) {
            mLogger.Debug() << "Font named " << arFontName << " has no style " << static_cast<int>(aStyle) << " installed.";
            aStyle = FontStyles::Normal;
        }
    }

    std::string filename = mFontSets[arFontName][aStyle].FileName;
    FT_Long id = mFontSets[arFontName][aStyle].Id;

    FT_Face result;
    FT_Error error = FT_New_Face(mFtLib, filename.c_str(), id, &result);
    if (error) {
        THROW_WITH_BACKTRACE2(FontException, "FT_New_Face() failed", error);
    }

    return result;
}

void FreeTypeLibrary::RegisterFont(const std::string &arFileName)
{
    FT_Face face = nullptr;
    FT_Long face_idx = 0;
    FT_Long instance_idx = 0;

    FontInfo info;
    info.FileName = arFileName;

    do {
        if (face) {
            FT_Done_Face(face);
        }

        FT_Long id = (instance_idx << 16) + face_idx;

        FT_Error error = FT_New_Face(mFtLib, arFileName.c_str(), id, &face);

        if (error) {
            THROW_WITH_BACKTRACE2(FontException, StrUtils::Format("FT_New_Face() failed trying to open %s", arFileName.c_str()).c_str(), error);
        }

        info.Id = id;
        info.StyleName = face->style_name;
        FontStyles style{};
        bool ignore = true;

        if (StrUtils::StartsWith(face->style_name, "Regular")) {
            ignore = false;
            if (face->style_flags & FT_STYLE_FLAG_ITALIC) {
                style = FontStyles::Italic;
            }
        }
        else if (StrUtils::StartsWith(face->style_name, "Bold")) {
            ignore = false;
            if (face->style_flags & FT_STYLE_FLAG_ITALIC) {
                style = FontStyles::BoldItalic;
            }
            else {
                style = FontStyles::Bold;
            }
        }

        if (ignore) {
            mLogger.Debug() << "Ignoring font " << face->family_name << ", " << face->style_name;
        }
        else {
            mLogger.Debug() << "Adding font " << face->family_name << ", " << face->style_name << " " << static_cast<int>(style);
            mFontSets[face->family_name][style] = info;
        }

#ifdef DEBUG_FONTS
        std::cout << "num_faces: " << face->num_faces << "\n"
            << "face_index: " << (face->face_index & 0xFFFF) << ", style:" << (face->face_index >> 16) << "\n"
            << "face_flags: " << std::hex << face->face_flags << std::dec << "\n"
            << "style_flags: " << std::hex << face->style_flags << std::dec << "\n"
            << "num_glyphs: " << std::hex << face->num_glyphs << std::dec << "\n"
            << "family_name: " << std::hex << face->family_name << std::dec << "\n"
            << "style_name: " << std::hex << face->style_name << std::dec << "\n"
            << "num_fixed_sizes: " << face->num_fixed_sizes << "\n"
            << "available_sizes: " << face->available_sizes << "\n"
            << "num_charmaps: " << face->num_charmaps << "\n"
            << "charmaps: " << face->charmaps << "\n";
#endif
        FT_Long num_instances = face->style_flags >> 16;

        if (instance_idx < num_instances) {
            instance_idx++;
        }
        else {
            face_idx++;
            instance_idx = 0;
        }
    } while (face_idx < face->num_faces);

    if (face) {
        FT_Done_Face(face);
    }
}

} // namespace rsp::graphics

#endif /* USE_FREETYPE */
