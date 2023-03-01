/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_PRIMITIVES_SDL_SDLTEXTURE_H_
#define SRC_GRAPHICS_PRIMITIVES_SDL_SDLTEXTURE_H_

#include <graphics/primitives/Texture.h>

namespace rsp::graphics {

class SDLTexture: public Texture
{
public:
    SDLTexture(const Renderer &arRenderer, const PixelData &arPixelData);

protected:
    void assign(const rsp::graphics::PixelData &arPixelData) override;
};

} /* namespace rsp::graphics */

#endif /* SRC_GRAPHICS_PRIMITIVES_SDL_SDLTEXTURE_H_ */
