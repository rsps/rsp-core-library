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
    Rect r = arRect & Rect(0, 0, GetWidth(), GetHeight());

    auto top = r.GetTop();
    auto left = r.GetLeft();
    auto right = r.GetRight() - 1;
    auto bottom = r.GetBottom() - 1;
    GuiUnit_t h_end = top + r.GetHeight();
    GuiUnit_t w_end = left + r.GetWidth();

    for (GuiUnit_t x = left; x < w_end; x++) {
        SetPixel(x, top, aColor); // top
        SetPixel(x, bottom, aColor); // bottom
    }
    for (GuiUnit_t y = top; y < h_end; y++) {
        SetPixel(left, y, aColor); // left
        SetPixel(right, y, aColor); // right
    }

    return *this;
}

Renderer& SWRenderer::Fill(Color aColor)
{
    for (std::uint32_t y = 0; y < mVariableInfo.yres; y++) {
        for (std::uint32_t x = 0; x < mVariableInfo.xres; x++) {
            std::size_t location = ((x + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + y * mFixedInfo.line_length) / sizeof(std::uint32_t);
            mpBackBuffer[location] = aColor;
        }
    }
    return *this;
}

Renderer& SWRenderer::FlushEvents()
{
    mTouchParser.Flush();
    return *this;
}

std::unique_ptr<Texture> SWRenderer::CreateTexture(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    return std::make_unique<SWTexture>(aWidth, aHeight, GetColorDepth());
}

std::unique_ptr<Texture> SWRenderer::CreateStaticTexture(const PixelData &arPixelData)
{
    auto t = std::make_unique<SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight(), GetColorDepth());

    t->Update(arPixelData);

    return t;
}

Renderer& SWRenderer::RenderTexture(const Texture &arTexture, const Rect &arDestination)
{
    const SWTexture& t = dynamic_cast<const SWTexture&>(arTexture);

    const PixelData &pd = t.GetPixelData();

    for (GuiUnit_t y = 0; y < pd.GetHeight(); ++y) {
        for (GuiUnit_t x = 0; x < pd.GetWidth(); ++x) {
            SetPixel(x, y, pd.GetPixelAt(x, y, Color::None));
        }
    }

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
    return mTouchParser.Poll(arTouchEvent);
}

} /* namespace rsp::graphics */
