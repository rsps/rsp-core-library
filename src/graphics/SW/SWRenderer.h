/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SW_SWRENDERER_H_
#define SRC_GRAPHICS_SW_SWRENDERER_H_

#include <graphics/Framebuffer.h>
#include <string>
#include <graphics/Renderer.h>
#include <utils/Singleton.h>

namespace rsp::graphics::sw {

class SWRenderer: public Renderer, public Framebuffer, public rsp::utils::Singleton<SWRenderer>
{
public:
    SWRenderer() {}

    GuiUnit_t GetWidth() const override;
    GuiUnit_t GetHeight() const override;
    PixelData::ColorDepth GetColorDepth() const override;

    Renderer& DrawRect(Color aColor, const Rect &arRect) override;
    Renderer& Fill(Color aColor, Optional<const Rect> aDestination = nullptr) override;
    Renderer& Blit(const Texture &arTexture) override;

    void Present() override;

    Renderer& SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) override;
    uint32_t GetPixel(GuiUnit_t aX, GuiUnit_t aY, bool aFront) const override;
};

} /* namespace rsp::graphics::sw */

#endif /* SRC_GRAPHICS_SW_SWRENDERER_H_ */