/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_SW_SWRENDERER_H_
#define INCLUDE_GRAPHICS_SW_SWRENDERER_H_

#include <string>
#include <graphics/Renderer.h>
#include <graphics/SW/Framebuffer.h>
#include <graphics/SW/SWTexture.h>

namespace rsp::graphics {

class SWRenderer: public Renderer, public Framebuffer
{
public:
    SWRenderer(const std::string &arRenderDevice);

    GuiUnit_t GetWidth() const override;
    GuiUnit_t GetHeight() const override;
    PixelData::ColorDepth GetColorDepth() const override;

    std::shared_ptr<Texture> CreateTexture(GuiUnit_t aWidth = 0, GuiUnit_t aHeight = 0) override;
    std::shared_ptr<Texture> CreateStaticTexture(const PixelData &arPixelData) override;

    Renderer& DrawRect(const Rect &arRect, Color aColor) override;
    Renderer& Fill(Color aColor) override;

    Renderer& Render(const Texture &arTexture, const Rect * const apDestination = nullptr) override;

    void Present() override;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_SW_SWRENDERER_H_ */
