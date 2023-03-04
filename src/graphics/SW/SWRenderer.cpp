/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <graphics/SW/SWRenderer.h>
#include <graphics/SW/SWTexture.h>

namespace rsp::graphics {

SWRenderer::SWRenderer(const std::string &arRenderDevice)
    : Framebuffer(arRenderDevice.c_str())
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

std::unique_ptr<Texture> SWRenderer::CreateTexture(GuiUnit_t aWidth, GuiUnit_t aHeight)
{
    if (!aWidth) {
        aWidth = GetWidth();
    }
    if (!aHeight) {
        aHeight = GetHeight();
    }
    return std::make_unique<SWTexture>(aWidth, aHeight, GetColorDepth());
}

std::unique_ptr<Texture> SWRenderer::CreateStaticTexture(const PixelData &arPixelData)
{
    auto t = std::make_unique<SWTexture>(arPixelData.GetWidth(), arPixelData.GetHeight(), GetColorDepth());

    t->Update(arPixelData, Color::Black);

    return t;
}

Renderer& SWRenderer::Render(const Texture &arTexture, const Rect * const apDestination)
{
    Rect dst = (apDestination) ? *apDestination : Rect(0, 0, GetWidth(), GetHeight());

    const SWTexture& t = dynamic_cast<const SWTexture&>(arTexture);

    const PixelData &pd = t.GetPixelData();
    GuiUnit_t h_end = std::min(pd.GetHeight(), dst.GetHeight());
    GuiUnit_t w_end = std::min(pd.GetWidth(), dst.GetWidth());
    GuiUnit_t dy = dst.GetTop();

    for (GuiUnit_t y = 0; y < h_end; ++y) {
        GuiUnit_t dx = dst.GetLeft();
        for (GuiUnit_t x = 0; x < w_end; ++x) {
            SetPixel(dx++, dy, pd.GetPixelAt(x, y, Color::None));
        }
        ++dy;
    }

    return *this;
}

void SWRenderer::Present()
{
    swapBuffer();
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

} /* namespace rsp::graphics */
