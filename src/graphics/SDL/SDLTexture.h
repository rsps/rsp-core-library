/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SDL_SDLTEXTURE_H_
#define SRC_GRAPHICS_SDL_SDLTEXTURE_H_

#ifdef USE_GFX_SDL

#include <graphics/Texture.h>

namespace rsp::graphics::sdl {

class SDLTexture: public rsp::graphics::Texture
{
public:
    SDLTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset);

    Texture& Blit(const Texture &arTexture) override;
    Texture& SetSourceRect(const Rect &arRect) override;
    GuiUnit_t GetWidth() const override;
    Rect GetDestinationRect() const override;
    const Rect& GetSourceRect() const override;
    Texture& DrawRect(Color aColor, const Rect &arRect) override;
    Texture& Update(const PixelData &arPixelData, Color aColor = Color::None) override;
    Texture& SetBlendOperation(Texture::BlendOperation aOp, Color aColorKey = Color::None) override;
    Texture& SetOffset(const Point &arPoint) override;
    Texture& SetDestination(const Point &arPoint) override;
    Texture& Fill(Color aColor, OptionalRect arRect = nullptr) override;
    Point GetDestination() const override;
    GuiUnit_t GetHeight() const override;
    TexturePtr_t Clone() const override;
};

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
#endif /* SRC_GRAPHICS_SDL_SDLTEXTURE_H_ */
