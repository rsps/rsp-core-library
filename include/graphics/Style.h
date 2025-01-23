/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_GRAPHICS_STYLE_H
#define RSP_CORE_LIB_GRAPHICS_STYLE_H

#include <vector>
#include <graphics/Color.h>
#include <graphics/Texture.h>

namespace rsp::graphics {

class Style
{
public:
    Style() noexcept = default;

    Style(const Style &arOther) {
        for(const TexturePtr_t &tex : arOther.mTextures) {
            mTextures.push_back(tex->Clone());
        }
    }
    Style& operator=(const Style &arOther) {
        if (this != &arOther) {
            for(const TexturePtr_t &tex : arOther.mTextures) {
               mTextures.push_back(tex->Clone());
            }
        }
        return *this;
    }
    Style(Style &&arOther) = default;
    Style& operator=(Style &&arOther) = default;

    Color mForegroundColor = Color::White;
    Color mBackgroundColor = Color::None;
    std::vector<TexturePtr_t> mTextures{};
};


} /* namespace rsp::graphics */

#endif // RSP_CORE_LIB_GRAPHICS_STYLE_H
