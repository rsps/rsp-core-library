/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */
#ifdef USE_GFX_SW

#include "SWTexture.h"

namespace rsp::graphics {

std::unique_ptr<Texture> Texture::Create(const PixelData &arPixelData, const Color &arColor, Point aDestPos, Point aDestOffset)
{
    auto result = std::make_unique<sw::SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight(), aDestPos, aDestOffset);
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
    return std::make_unique<sw::SWTexture>(aWidth, aHeight, aDestPos, aDestOffset);
}

} /* namespace rsp::graphics */


namespace rsp::graphics::sw {

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset)
    : mrGfxHal(GfxHal::Get()),
      mSourceRect(0, 0, aWidth, aHeight),
      mDestinationPos(arDestPos),
      mDestinationOffset(arDestOffset)
{
    mpSurface = mrGfxHal.Alloc(aWidth, aHeight);
}

Texture& SWTexture::Fill(const Color &arColor, GfxHal::OptionalRect aRect)
{
    mrGfxHal.Fill(*mpSurface, arColor.AsRaw(), aRect);
    return *this;
}

Texture& SWTexture::Update(const PixelData &arPixelData, const Color &arColor)
{
    mrGfxHal.CopyFrom(*mpSurface, arPixelData, arColor.AsRaw());
    return *this;
}

Texture& SWTexture::SetBlendOperation(Texture::BlendOperation aOp, const Color &arColorKey)
{
    mpSurface->mBlendOperation = aOp;
    mpSurface->mColorKey = arColorKey.AsRaw();
    return *this;
}

Texture& SWTexture::SetSourceRect(const Rect &arRect)
{
    mSourceRect = arRect & Rect(0, 0, GetWidth(), GetHeight());
    return *this;
}

const Rect& SWTexture::GetSourceRect() const
{
    return mSourceRect;
}

Point SWTexture::GetDestination() const
{
    return mDestinationPos;
}

Texture& SWTexture::SetDestination(const Point &arPoint)
{
    mDestinationPos = arPoint;
    return *this;
}

Texture& SWTexture::SetOffset(const Point &arPoint)
{
    mDestinationOffset = arPoint;
    return *this;
}

Rect SWTexture::GetDestinationRect() const
{
    Rect result(mSourceRect);
    result.MoveTo({mDestinationPos.GetX() + mDestinationOffset.GetX(), mDestinationPos.GetY() + mDestinationOffset.GetY()});
    return result;
}

std::unique_ptr<Texture> SWTexture::Clone() const
{
    return std::unique_ptr<SWTexture>(new SWTexture(*this));
}

GuiUnit_t SWTexture::GetWidth() const
{
    return mpSurface->mWidth;
}

rsp::graphics::GuiUnit_t SWTexture::GetHeight() const
{
    return mpSurface->mHeight;
}


} /* namespace rsp::graphics::sw */

#endif /* USE_GFX_SW */
