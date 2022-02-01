/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPELIBRARY_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPELIBRARY_H_

#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace rsp::graphics {

/**
 * \class FreeTypeLibrary
 * \brief Simple object to load the freetype library
 *
 */
class FreeTypeLibrary
{
public:
    static FreeTypeLibrary& Get();

    void RegisterFont(const std::string &arFileName);

    operator FT_Library() const {
        return mFtLib;
    }

    const char* GetFileName(const std::string &arFontName) const;

private:
    FreeTypeLibrary(void);
    ~FreeTypeLibrary(void);
    FreeTypeLibrary(const FreeTypeLibrary&) = delete;
    FreeTypeLibrary& operator =(const FreeTypeLibrary&) = delete;

    FT_Library mFtLib { };

    std::map<std::string, std::string> mFontFileNames{ };
};

}

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_FREETYPE_FREETYPELIBRARY_H_ */
