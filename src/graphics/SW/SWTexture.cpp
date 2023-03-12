/*!
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \author      steffen
 */

#include <graphics/SW/SWTexture.h>

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
    arPixelData.CopyToSurface(mSurface);
}

} /* namespace rsp::graphics::sw */
