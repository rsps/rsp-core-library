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

std::unique_ptr<Texture::Interface> Texture::Interface::Create(const PixelData &arPixelData, Color aColor)
{
    auto result = std::make_unique<sw::SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight());
    result->Update(arPixelData, aColor);
    return result;
}

std::unique_ptr<Texture::Interface> Texture::Interface::Create(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    return std::make_unique<sw::SWTexture>(aWidth, aHeight);
}

} /* namespace rsp::graphics */


namespace rsp::graphics::sw {

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight)
    : mrGfxHal(GfxHal::Get()),
      mSourceRect(0, 0, aWidth, aHeight),
      mDestinationRect(0, 0, aWidth, aHeight)
{
    mpSurface = std::make_shared<VideoSurface>(aWidth, aHeight);
}

void SWTexture::Fill(Color aColor, GfxHal::Optional<const Rect> aRect)
{
    mrGfxHal.Fill(*mpSurface, aColor, aRect);
}

void SWTexture::Update(const PixelData &arPixelData, Color aColor)
{
    mrGfxHal.CopyFrom(*mpSurface, arPixelData, aColor);
}

void SWTexture::SetBlendOperation(GfxBlendOperation aOp, Color aColorKey)
{
    mpSurface->mBlendOperation = aOp;
    mpSurface->mColorKey = aColorKey;
}

void SWTexture::SetSourceRect(const Rect &arRect)
{
    mSourceRect = arRect;
    mDestinationRect.SetWidth(arRect.GetWidth());
    mDestinationRect.SetHeight(arRect.GetHeight());
}

void SWTexture::SetDestination(const Point &arPoint)
{
    mDestinationRect.MoveTo(arPoint);
}

void SWTexture::render(VideoSurface &arSurface) const
{
    mrGfxHal.Blit(arSurface, *mpSurface, mDestinationRect, mSourceRect);

}

std::unique_ptr<Texture::Interface> SWTexture::Clone() const
{
    return std::make_unique<Texture>(*this);
}

} /* namespace rsp::graphics::sw */
