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

std::unique_ptr<Texture> Texture::Create(const PixelData &arPixelData, Color aColor, Point aDestPos)
{
    auto result = std::make_unique<sw::SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight(), aDestPos);
    result->Update(arPixelData, aColor);
    return result;
}

std::unique_ptr<Texture> Texture::Create(GuiUnit_t aWidth, GuiUnit_t aHeight, Point aDestPos)
{
    return std::make_unique<sw::SWTexture>(aWidth, aHeight, aDestPos);
}

} /* namespace rsp::graphics */


namespace rsp::graphics::sw {

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos)
    : mrGfxHal(GfxHal::Get()),
      mSourceRect(0, 0, aWidth, aHeight),
      mDestinationRect(arDestPos, aWidth, aHeight)
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
    mDestinationRect.SetWidth(arRect.GetWidth());
    mDestinationRect.SetHeight(arRect.GetHeight());
    return *this;
}

Texture& SWTexture::SetDestination(const Point &arPoint)
{
    mDestinationRect.MoveTo(arPoint);
    return *this;
}

std::unique_ptr<Texture> SWTexture::Clone() const
{
    return std::unique_ptr<SWTexture>(new SWTexture(*this));
}

} /* namespace rsp::graphics::sw */
