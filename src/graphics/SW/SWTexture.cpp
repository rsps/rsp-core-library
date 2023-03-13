/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <graphics/SW/SWTexture.h>

namespace rsp::graphics {

std::shared_ptr<Texture> Texture::Create(const PixelData &arPixelData, Color aColor)
{
    auto result = std::make_shared<sw::SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight());
    result->Update(arPixelData, aColor);
    return result;
}

std::shared_ptr<Texture> Texture::Create(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    return std::make_shared<sw::SWTexture>(aWidth, aHeight);
}

} /* namespace rsp::graphics */


namespace rsp::graphics::sw {

SWTexture::SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight)
    : mrGfxHal(GfxHal::Get())
{
    mrGfxHal.Alloc(mSurface, aWidth, aHeight);
}

void SWTexture::Fill(Color aColor, GfxHal::Optional<const Rect> aRect)
{
    mrGfxHal.Fill(mSurface, aColor, aRect);
}

void SWTexture::Update(const PixelData &arPixelData, Color aColor)
{
    mrGfxHal.CopyFrom(mSurface, arPixelData, aColor);
}

void SWTexture::SetBlendOperation(GfxBlendOperation aOp, Color aColorKey)
{
    mSurface.mBlendOperation = aOp;
    mSurface.mColorKey = aColorKey;
}

} /* namespace rsp::graphics::sw */
