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
#include <posix/FileSystem.h>
#include "SWRenderer.h"
#include "SWTexture.h"
#include "GfxHal.h"

namespace rsp::graphics {

void Renderer::SetDevicePath(const std::string &arPath)
{
    sw::Framebuffer::mpDevicePath = arPath.empty() ? nullptr : arPath.c_str();
}

Renderer& Renderer::Init(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    if (!sw::SWRenderer::HasInstance()) {
        sw::SWRenderer::CreateInstance(aWidth, aHeight);
    }
    return rsp::utils::Singleton<sw::SWRenderer>::GetInstance();
}

Renderer& Renderer::Get()
{
    return rsp::utils::Singleton<sw::SWRenderer>::GetInstance();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sw {

SWRenderer::SWRenderer(GuiUnit_t aWidth, GuiUnit_t aHeight)
    : mClipRect(0, 0, Framebuffer::GetWidth(), Framebuffer::GetHeight())
{
    if (aWidth != Framebuffer::GetWidth() || aHeight != Framebuffer::GetHeight()) {
        // throw...
    }
}

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
    Rect src = swt.mSourceRect;
    if (dst.GetTop() < 0) {
        src.SetTop(std::min(-dst.GetTop(), src.GetHeight()));
    }
    if (dst.GetLeft() < 0) {
        src.SetLeft(std::min(-dst.GetLeft(), src.GetWidth()));
    }

    int dy = 0;
    int dx = 0;
    if (arTexture.GetWidth() > GetWidth() && src.GetWidth() < GetWidth() && src.GetLeft() == 0) {
        dx = GetWidth() - src.GetWidth();
    }
    if (arTexture.GetHeight() > GetHeight() && src.GetHeight() < GetHeight() && src.GetTop() == 0) {
        dy = GetHeight() - src.GetHeight();
    }
    dst.Move(dx, dy);

    dst &= mClipRect;

    mrGfxHal.Blit(mScreenSurfaces[mCurrentSurface], *swt.mpSurface, dst, src);
    return *this;
}

Renderer& SWRenderer::SetClipRect(const Rect &arClipRect)
{
    mClipRect = arClipRect & Rect(0, 0, GetWidth(), GetHeight());
    return *this;
}

Renderer& SWRenderer::ClearClipRect()
{
    mClipRect = Rect(0, 0, GetWidth(), GetHeight());
    return *this;
}

Renderer& SWRenderer::Flush()
{
    mrGfxHal.Sync();
    return *this;
}

void SWRenderer::Present()
{
    Flush();
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
