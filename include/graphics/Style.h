/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2022 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_STYLE_H_
#define INCLUDE_GRAPHICS_STYLE_H_

#include <memory>
#include <vector>
#include <graphics/Color.h>
#include <graphics/Texture.h>

namespace rsp::graphics {

class Style
{
public:
    Color mForegroundColor = Color::White;
    Color mBackgroundColor = Color::None;
    std::vector<TexturePtr_t> mTextures{};
};


} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_STYLE_H_ */
