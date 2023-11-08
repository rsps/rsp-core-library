/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_GRAPHICS_FREETYPE_FREE_TYPE_LIBRARY_H
#define RSP_CORE_LIB_SRC_GRAPHICS_FREETYPE_FREE_TYPE_LIBRARY_H

#ifdef USE_FREETYPE

#include <string>
#include <map>
#include <graphics/Font.h>
#include <logging/LogChannel.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace rsp::graphics {

struct FontInfo {
    std::string FileName{};
    std::string StyleName{};
    FT_Long Id = 0;
};

/**
 * \class FreeTypeLibrary
 * \brief Simple object to load the freetype library
 *
 */
class FreeTypeLibrary: public logging::NamedLogger<FreeTypeLibrary>
{
public:
    FreeTypeLibrary(const FreeTypeLibrary&) = delete;
    FreeTypeLibrary& operator =(const FreeTypeLibrary&) = delete;

    static FreeTypeLibrary& Get();

    void RegisterFont(const std::string &arFileName);

    explicit operator FT_Library() const {
        return mFtLib;
    }

    FT_Face CreateFontFace(const std::string &arFontName, FontStyles aStyle);

private:
    FreeTypeLibrary();
    ~FreeTypeLibrary() override;

    FT_Library mFtLib { };

    std::map<std::string, std::map<FontStyles, FontInfo> > mFontSets{ };
};

}

#endif /* USE_FREETYPE */

#endif // RSP_CORE_LIB_SRC_GRAPHICS_FREETYPE_FREE_TYPE_LIBRARY_H
