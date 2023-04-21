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
#include "GfxHal.h"

namespace rsp::graphics {

Renderer& Renderer::Get()
{
    if (!sw::SWRenderer::HasInstance()) {
        sw::SWRenderer::CreateInstance();
    }
    return rsp::utils::Singleton<sw::SWRenderer>::GetInstance();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sw {

Renderer& SWRenderer::DrawRect(const Color &arColor, const Rect &arRect)
{
    mrGfxHal.DrawRect(mScreenSurfaces[mCurrentSurface], arColor.AsRaw(), arRect);
    return *this;
}

Renderer& SWRenderer::Fill(const Color &arColor, OptionalRect aDestination)
{
    mrGfxHal.Fill(mScreenSurfaces[mCurrentSurface], arColor.AsRaw(), aDestination);
    return *this;
}

Renderer& SWRenderer::Blit(const Texture &arTexture)
{
    auto &swt = dynamic_cast<const SWTexture&>(arTexture);
    Rect dst = swt.GetDestinationRect();
    dst &= mClipRect;
    mrGfxHal.Blit(mScreenSurfaces[mCurrentSurface], *swt.mpSurface, dst, swt.mSourceRect);
    return *this;
}

Renderer& SWRenderer::SetClipRect(const Rect &arClipRect)
{
    mClipRect = arClipRect & Rect(0, 0, GetWidth(), GetHeight());
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

ColorDepth SWRenderer::GetColorDepth() const
{
    switch (mVariableInfo.bits_per_pixel) {
        case 32:
            return ColorDepth::RGBA;

        default:
        case 1:
        case 24:
            ;
    }
    THROW_WITH_BACKTRACE(EIllegalColorDepth);
}

Renderer& SWRenderer::SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor)
{
    mrGfxHal.SetPixel(mScreenSurfaces[mCurrentSurface], aX, aY, arColor.AsRaw());
    return *this;
}

Color SWRenderer::GetPixel(GuiUnit_t aX, GuiUnit_t aY) const
{
    Color result;
    result.FromRaw(mrGfxHal.GetPixel(mScreenSurfaces[mCurrentSurface], aX, aY));
    return result;
}

} /* namespace rsp::graphics::sw */

#endif /* USE_GFX_SW */
