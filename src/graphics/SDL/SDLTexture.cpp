/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef USE_GFX_SDL

#include "SDLTexture.h"

namespace rsp::graphics {

std::unique_ptr<Texture> Texture::Create(const PixelData &arPixelData, Color aColor, Point aDestPos, Point aDestOffset)
{
    auto result = std::make_unique<sdl::SDLTexture>(arPixelData.GetWidth(), arPixelData.GetHeight(), aDestPos, aDestOffset);
    switch(arPixelData.GetColorDepth()) {
        default:
        case ColorDepth::RGB:
        case ColorDepth::RGBA:
            result->SetBlendOperation(Texture::BlendOperation::Copy);
            break;
        case ColorDepth::Monochrome:
        case ColorDepth::Alpha:
            result->SetBlendOperation(Texture::BlendOperation::SourceAlpha);
            break;
    }
    result->Fill(Color::None).Update(arPixelData, aColor.AsRaw());
    return result;
}

std::unique_ptr<Texture> Texture::Create(GuiUnit_t aWidth, GuiUnit_t aHeight, Point aDestPos, Point aDestOffset)
{
    return std::make_unique<sdl::SDLTexture>(aWidth, aHeight, aDestPos, aDestOffset);
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sdl {

SDLTexture::SDLTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset)
{

}

Texture& SDLTexture::Blit(const Texture &arTexture)
{
    return *this;
}

Texture& SDLTexture::SetSourceRect(const Rect &arRect)
{
    return *this;
}

GuiUnit_t SDLTexture::GetWidth() const
{
}

Rect SDLTexture::GetDestinationRect() const
{
}

const Rect& SDLTexture::GetSourceRect() const
{
}

Texture& SDLTexture::DrawRect(Color aColor, const Rect &arRect)
{
    return *this;
}

Texture& SDLTexture::Update(const PixelData &arPixelData, Color aColor)
{
    return *this;
}

Texture& SDLTexture::SetBlendOperation(Texture::BlendOperation aOp,
    Color aColorKey)
{
    return *this;
}

Texture& SDLTexture::SetOffset(const Point &arPoint)
{
    return *this;
}

Texture& SDLTexture::SetDestination(const Point &arPoint)
{
    return *this;
}

Texture& SDLTexture::Fill(Color aColor, OptionalRect arRect)
{
    return *this;
}

Point SDLTexture::GetDestination() const
{
}

GuiUnit_t SDLTexture::GetHeight() const
{
}

TexturePtr_t SDLTexture::Clone() const
{
}

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
