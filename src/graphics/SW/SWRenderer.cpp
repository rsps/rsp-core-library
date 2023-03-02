/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include "SWRenderer.h"
#include "SWTexture.h"

namespace rsp::graphics {

SWRenderer::SWRenderer(const std::string &arRenderDevice, const std::string &arTouchDevice)
    : Framebuffer(arRenderDevice.c_str()),
      mTouchParser(arTouchDevice)
{
}

Renderer& SWRenderer::DrawRect(const Rect &arRect, Color aColor)
{
    return *this;
}

Renderer& SWRenderer::Fill(Color aColor)
{
    return *this;
}

Renderer& SWRenderer::FlushEvents()
{
    return *this;
}

std::unique_ptr<Texture> SWRenderer::CreateTexture(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    return std::make_unique<SWTexture>(aWidth, aHeight);
}

std::unique_ptr<Texture> SWRenderer::CreateStaticTexture(const PixelData &arPixelData)
{
    auto t = std::make_unique<SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight());

    t->Update(arPixelData);

    return t;
}

Renderer& SWRenderer::RenderTexture(const Texture &arTexture, const Rect &arDestination)
{
    return *this;
}

void SWRenderer::Present()
{
    SwapBuffer();
}

GuiUnit_t SWRenderer::GetHeight() const
{
    return GuiUnit_t(mVariableInfo.yres);
}

GuiUnit_t SWRenderer::GetWidth() const
{
    return GuiUnit_t(mVariableInfo.xres);
}

PixelData::ColorDepth SWRenderer::GetColorDepth() const
{
    switch (mVariableInfo.bits_per_pixel) {
        case 1:
            return PixelData::ColorDepth::Monochrome;
        case 24:
            return PixelData::ColorDepth::RGB;
        case 32:
            return PixelData::ColorDepth::RGBA;
        default:
            ;
    }
    THROW_WITH_BACKTRACE(EIllegalColorDepth);
}

bool SWRenderer::PollEvents(TouchEvent &arTouchEvent)
{
}

} /* namespace rsp::graphics */
