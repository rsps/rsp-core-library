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

#include <cstring>
#include "SWRenderer.h"
#include "SWTexture.h"
#include <graphics/GfxHal.h>

namespace rsp::graphics {

Renderer& Renderer::Get()
{
    if (!sw::SWRenderer::HasInstance()) {
        sw::SWRenderer::Create();
    }
    return rsp::utils::Singleton<sw::SWRenderer>::Get();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sw {

Renderer& SWRenderer::DrawRect(Color aColor, const Rect &arRect)
{
    mrGfxHal.DrawRect(mScreenSurfaces[mCurrentSurface], aColor, arRect);
    return *this;
}

Renderer& SWRenderer::Fill(Color aColor, Optional<const Rect> aDestination)
{
    mrGfxHal.Fill(mScreenSurfaces[mCurrentSurface], aColor, aDestination);
    return *this;
}

Renderer& SWRenderer::Blit(const Texture &arTexture)
{
    auto &swt = dynamic_cast<const SWTexture&>(arTexture);
    mrGfxHal.Blit(mScreenSurfaces[mCurrentSurface], *swt.mpSurface, swt.GetDestinationRect(), swt.mSourceRect);
    return *this;
}

void SWRenderer::Present()
{
    mrGfxHal.Sync();
    swapBuffer();
}

GuiUnit_t SWRenderer::GetHeight() const
{
    return Framebuffer::GetHeight();
}

GuiUnit_t SWRenderer::GetWidth() const
{
    return Framebuffer::GetWidth();
}

PixelData::ColorDepth SWRenderer::GetColorDepth() const
{
    switch (mVariableInfo.bits_per_pixel) {
        case 32:
            return PixelData::ColorDepth::RGBA;

        default:
        case 1:
        case 24:
            ;
    }
    THROW_WITH_BACKTRACE(EIllegalColorDepth);
}

Renderer& SWRenderer::SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
{
    mrGfxHal.SetPixel(mScreenSurfaces[mCurrentSurface], aX, aY, arColor);
    return *this;
}

uint32_t SWRenderer::GetPixel(GuiUnit_t aX, GuiUnit_t aY, bool aFront) const
{
    if (aFront) {
        return mrGfxHal.GetPixel(mScreenSurfaces[(mCurrentSurface) ? 0 : 1], aX, aY);
    }
    return mrGfxHal.GetPixel(mScreenSurfaces[mCurrentSurface], aX, aY);
}

} /* namespace rsp::graphics::sw */

#endif /* USE_GFX_SW */
