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
    mrGfxHal.DrawRect(mScreenSurfaces[mCurrentSurface], aColor, arRect);
    return *this;
}

Renderer& SWRenderer::Fill(Color aColor, Optional<const Rect> aDestination)
{
    mrGfxHal.Fill(mScreenSurfaces[mCurrentSurface], aColor, aDestination);
    return *this;
}

std::shared_ptr<Texture> SWRenderer::CreateTexture(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    if (!aWidth) {
        aWidth = GetWidth();
    }
    if (!aHeight) {
        aHeight = GetHeight();
    }
    return std::make_shared<SWTexture>(aWidth, aHeight);
}

std::shared_ptr<const Texture> SWRenderer::CreateStaticTexture(const PixelData &arPixelData)
{
    auto t = std::make_shared<SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight());

    t->Update(arPixelData, Color::Black);

    return t;
}

Renderer& SWRenderer::Render(const Texture &arTexture, Optional<const Rect> aDestination, Optional<const Rect> aSource)
{
    const SWTexture& t = dynamic_cast<const SWTexture&>(arTexture);

    mrGfxHal.Blit(mScreenSurfaces[mCurrentSurface], t.getSurface(), aDestination, aSource);
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
