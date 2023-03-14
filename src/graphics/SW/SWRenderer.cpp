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
#include <graphics/SW/SWRenderer.h>
#include <graphics/SW/SWTexture.h>
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
    mrGfxHal.DrawRect(*mScreenSurfaces[mCurrentSurface], aColor, arRect);
    return *this;
}

Renderer& SWRenderer::Fill(Color aColor, Optional<const Rect> aDestination)
{
    mrGfxHal.Fill(*mScreenSurfaces[mCurrentSurface], aColor, aDestination);
    return *this;
}

Renderer& SWRenderer::Render(const Texture &arTexture)
{
    const SWTexture& t = dynamic_cast<const SWTexture&>(arTexture);
    t.render(*mScreenSurfaces[mCurrentSurface]);
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

} /* namespace rsp::graphics::sw */

#endif /* USE_GFX_SW */
