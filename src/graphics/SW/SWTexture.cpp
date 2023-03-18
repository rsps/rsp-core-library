/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/SW/SWTexture.h>

namespace rsp::graphics {

std::unique_ptr<Texture> Texture::Create(const PixelData &arPixelData, Color aColor, Point aDestPos, Point aDestOffset)
{
    auto result = std::make_unique<sw::SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight(), aDestPos, aDestOffset);
    switch(arPixelData.GetColorDepth()) {
        default:
        case PixelData::ColorDepth::RGB:
        case PixelData::ColorDepth::RGBA:
            result->SetBlendOperation(GfxBlendOperation::Copy);
            break;
        case PixelData::ColorDepth::Monochrome:
        case PixelData::ColorDepth::Alpha:
            result->SetBlendOperation(GfxBlendOperation::SourceAlpha);
            break;
    }
    result->Fill(Color::None).Update(arPixelData, aColor);
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
    mpSurface = std::make_shared<VideoSurface>(aWidth, aHeight);
}

Texture& SWTexture::Fill(Color aColor, GfxHal::Optional<const Rect> aRect)
{
    mrGfxHal.Fill(*mpSurface, aColor, aRect);
    return *this;
}

Texture& SWTexture::Update(const PixelData &arPixelData, Color aColor)
{
    mrGfxHal.CopyFrom(*mpSurface, arPixelData, aColor);
    return *this;
}

Texture& SWTexture::SetBlendOperation(GfxBlendOperation aOp, Color aColorKey)
{
    mpSurface->mBlendOperation = aOp;
    mpSurface->mColorKey = aColorKey;
    return *this;
}

Texture& SWTexture::SetSourceRect(const Rect &arRect)
{
    mSourceRect = arRect;
    return *this;
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

Rect SWTexture::GetDestinationRect() const
{
    Rect result(mSourceRect);
    result.Move(mDestinationPos.GetX() + mDestinationOffset.GetX(), mDestinationPos.GetY() + mDestinationOffset.GetY());
    return result;
}

Point SWTexture::GetDestination() const
{
    return mDestinationPos;
}

} /* namespace rsp::graphics::sw */
