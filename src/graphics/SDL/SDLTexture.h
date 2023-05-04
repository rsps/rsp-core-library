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

#include <memory>
#include <SDL2/SDL.h>
#include <graphics/Texture.h>

namespace rsp::graphics::sdl {

class SDLRenderer;

struct SDL_TextureWrapper
{
    SDL_TextureWrapper(SDL_Texture *apTexture, GuiUnit_t aWidth, GuiUnit_t aHeight) noexcept;
    ~SDL_TextureWrapper();
    SDL_TextureWrapper(const SDL_TextureWrapper&) = default;
    SDL_TextureWrapper(SDL_TextureWrapper&&) = default;
    SDL_TextureWrapper& operator=(const SDL_TextureWrapper&) = default;
    SDL_TextureWrapper& operator=(SDL_TextureWrapper&&) = default;

    SDL_Texture* Get() const { return mpTexture; }

protected:
    SDL_Texture *mpTexture;
    size_t mSize;
    static size_t mTotalAllocated;
};

class SDLTexture: public rsp::graphics::Texture
{
public:
    SDLTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset);
    ~SDLTexture() override;

    Texture& SetSourceRect(const Rect &arRect) override;
    GuiUnit_t GetWidth() const override;
    Rect GetDestinationRect() const override;
    const Rect& GetSourceRect() const override;
    Texture& Update(const PixelData &arPixelData, const Color &arColor = Color::None) override;
    Texture& SetBlendOperation(Texture::BlendOperation aOp, const Color &arColorKey = Color::None) override;
    Texture& SetOffset(const Point &arPoint) override;
    Texture& SetDestination(const Point &arPoint) override;
    Texture& Fill(const Color &arColor, OptionalRect arRect = nullptr) override;
    Point GetDestination() const override;
    GuiUnit_t GetHeight() const override;
    TexturePtr_t Clone() const override;

    SDL_Texture* GetSDLTexture() const { return mpTexture->Get(); }

protected:
    std::shared_ptr<SDL_TextureWrapper> mpTexture = nullptr;
    Rect mArea;
    Rect mSourceRect;
    Point mDestinationPos;
    Point mDestinationOffset;
    SDLRenderer &mrRenderer;

    friend class SDLRenderer;
    int mRotation = 0; // Rotation of this texture. Only supports: 0, 90, 180, 270
    Texture::BlendOperation mBlendOperation = Texture::BlendOperation::Copy;
    uint32_t mColorKey = Color::None; // Value of transparent pixel on this surface (source key)
};

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
#endif /* SRC_GRAPHICS_SDL_SDLTEXTURE_H_ */
