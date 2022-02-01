/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifndef INCLUDE_GRAPHICS_PRIMITIVES_FONTRAWINTERFACE_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_FONTRAWINTERFACE_H_

#include <string>
#include <graphics/primitives/Font.h>

namespace rsp::graphics {


class FontRawInterface
{
public:
    virtual ~FontRawInterface();

    virtual std::vector<Glyph> MakeGlyphs(const std::string &arText, int aLineSpacing) const = 0;
    virtual std::string GetFamilyName() const = 0;
    virtual void SetSize(int aWidthPx, int aHeightPx) = 0;

    int GetSize() const { return mSizePx; }

    virtual Font& SetStyle(Font::Styles aStyle);
    Font::Styles GetStyle() const { return mStyle; }

protected:
    Font::Styles mStyle;
    int mSizePx;
};

}

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_FONTRAWINTERFACE_H_ */
