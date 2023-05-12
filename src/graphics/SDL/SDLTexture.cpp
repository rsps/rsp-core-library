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
#include "SDLRenderer.h"
#include "SDLRect.h"
#include <logging/Logger.h>

namespace rsp::graphics {

std::unique_ptr<Texture> Texture::Create(const PixelData &arPixelData, const Color &arColor, Point aDestPos, Point aDestOffset)
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
    result->Fill(Color::None).Update(arPixelData, arColor.AsRaw());
    return result;
}

std::unique_ptr<Texture> Texture::Create(GuiUnit_t aWidth, GuiUnit_t aHeight, Point aDestPos, Point aDestOffset)
{
    return std::make_unique<sdl::SDLTexture>(aWidth, aHeight, aDestPos, aDestOffset);
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sdl {

size_t SDL_TextureWrapper::mTotalAllocated = 0;

SDL_TextureWrapper::SDL_TextureWrapper(SDL_Texture *apTexture, GuiUnit_t aWidth, GuiUnit_t aHeight) noexcept
    : mpTexture(apTexture),
      mSize(aWidth * aHeight * sizeof(uint32_t))
{
    if (!apTexture) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_CreateTexture() returned nullptr");
    }
    mTotalAllocated += mSize;
    logging::Logger::GetDefault().Debug() << "VideoMemAlloc(" << mSize << "), Total: " << (mTotalAllocated / 1024.0 / 1024.0) << " MB";
}

SDL_TextureWrapper::~SDL_TextureWrapper()
{
    if (mpTexture) {
        SDL_DestroyTexture(mpTexture);
        mTotalAllocated -= mSize;
        logging::Logger::GetDefault().Debug() << "VideoMemFree(" << mSize << "), Total: " << (mTotalAllocated / 1024.0 / 1024.0) << " MB";
    }
}


SDLTexture::SDLTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset)
    : mArea(0, 0, aWidth, aHeight),
      mSourceRect(0, 0, aWidth, aHeight),
      mDestinationPos(arDestPos),
      mDestinationOffset(arDestOffset),
      mrRenderer(dynamic_cast<SDLRenderer&>(Renderer::Get()))
{
    mpTexture = std::make_shared<SDL_TextureWrapper>(SDL_CreateTexture(mrRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, aWidth, aHeight), aWidth, aHeight);
    if (!mpTexture) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_CreateTexture");
    }
}

SDLTexture::~SDLTexture()
{
}

constexpr uint32_t* offset(uint32_t *addr, uintptr_t pitch, uintptr_t x, uintptr_t y) {
    return reinterpret_cast<uint32_t*>(uintptr_t(addr) + (y * pitch) + (x * sizeof(uint32_t)));
}

constexpr uint32_t* offset(uint32_t *addr, uintptr_t pitch, GuiUnit_t x, GuiUnit_t y) {
    return offset(addr, pitch, uintptr_t(x), uintptr_t(y));
}

Texture& SDLTexture::Update(const PixelData &arPixelData, const Color &arColor)
{
    uint32_t *dst;
    int pitch;
    if (SDL_LockTexture(mpTexture->Get(), nullptr, reinterpret_cast<void**>(&dst), &pitch)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_LockTexture");
    }

    Rect dr(mArea);
    Rect sr(arPixelData.GetRect());

    GuiUnit_t y_end = uintptr_t(std::min(dr.GetHeight(), sr.GetHeight()));
    size_t w = size_t(std::min(dr.GetWidth(), sr.GetWidth()));
    GuiUnit_t src_y = sr.GetTop();

    switch(arPixelData.GetColorDepth()) {
        default:
        case ColorDepth::RGB:
        case ColorDepth::RGBA: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, uintptr_t(pitch), dr.GetLeft(), y);
                GuiUnit_t src_x = sr.GetLeft();
                for(size_t x = 0; x < w ; ++x) {
                    uint32_t cl = arPixelData.GetPixelAt(src_x++, src_y, arColor).AsRaw();
                    if (mBlendOperation == Texture::BlendOperation::ColorKey && cl == mColorKey) {
                        continue;
                    }
                    dest[x] = cl;
                }
                ++src_y;
            }
            break;
        }

        case ColorDepth::Monochrome:
        case ColorDepth::Alpha: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, uintptr_t(pitch), dr.GetLeft(), y);
                GuiUnit_t src_x = sr.GetLeft();
                for(size_t x = 0; x < w ; ++x) {
                    uint32_t cl = arPixelData.GetPixelAt(src_x++, src_y, arColor).AsRaw();
                    if (mBlendOperation == Texture::BlendOperation::ColorKey && cl == mColorKey) {
                        continue;
                    }
                    if (cl & 0xFF000000) {
                        dest[x] = cl;
                    }
                }
                ++src_y;
            }
            break;
        }
    }

    SDL_UnlockTexture(mpTexture->Get());

    return *this;
}

Texture& SDLTexture::Fill(const Color &arColor, OptionalRect arRect)
{
    uint32_t *dst;
    int pitch;
    if (SDL_LockTexture(mpTexture->Get(), nullptr, reinterpret_cast<void**>(&dst), &pitch)) {
        THROW_WITH_BACKTRACE1(SDLException, "SDL_LockTexture");
    }

    Rect dr(mArea);
    if (arRect) {
        dr &= *arRect;
    }

    GuiUnit_t h = dr.GetHeight();
    size_t w = size_t(dr.GetWidth());
    uint32_t cl = arColor.AsRaw();

    for (GuiUnit_t y = dr.GetTop(); y < h ; ++y) {
        uint32_t *dest = offset(dst, uintptr_t(pitch), dr.GetLeft(), y);
        for(size_t x = 0; x < w ; ++x) {
            dest[x] = cl;
        }
    }

    SDL_UnlockTexture(mpTexture->Get());
    return *this;
}

Texture& SDLTexture::SetBlendOperation(Texture::BlendOperation aOp, const Color &arColorKey)
{
    mBlendOperation = aOp;
    mColorKey = arColorKey;

    if (mBlendOperation == Texture::BlendOperation::Copy) {
        if (SDL_SetTextureBlendMode(mpTexture->Get(), SDL_BLENDMODE_NONE)) {
            THROW_WITH_BACKTRACE1(SDLException, "SDL_SetTextureBlendMode");
        }
    }
    else {
        if (SDL_SetTextureBlendMode(mpTexture->Get(), SDL_BLENDMODE_BLEND)) {
            THROW_WITH_BACKTRACE1(SDLException, "SDL_SetTextureBlendMode");
        }
    }

    return *this;
}

Texture& SDLTexture::SetOffset(const Point &arPoint)
{
    mDestinationOffset = arPoint;
    return *this;
}

Texture& SDLTexture::SetSourceRect(const Rect &arRect)
{
    mSourceRect = arRect & mArea;
    return *this;
}

Rect SDLTexture::GetDestinationRect() const
{
    Rect result(mSourceRect);
    result.MoveTo({mDestinationPos.GetX() + mDestinationOffset.GetX(), mDestinationPos.GetY() + mDestinationOffset.GetY()});
    return result;
}

const Rect& SDLTexture::GetSourceRect() const
{
    return mSourceRect;
}

Texture& SDLTexture::SetDestination(const Point &arPoint)
{
    mDestinationPos = arPoint;
    return *this;
}

Point SDLTexture::GetDestination() const
{
    return mDestinationPos;
}

GuiUnit_t SDLTexture::GetWidth() const
{
    return mArea.GetWidth();
}

GuiUnit_t SDLTexture::GetHeight() const
{
    return mArea.GetHeight();
}

TexturePtr_t SDLTexture::Clone() const
{
    return std::unique_ptr<SDLTexture>(new SDLTexture(*this));
}

} /* namespace rsp::graphics::sdl */

#endif /* USE_GFX_SDL */
