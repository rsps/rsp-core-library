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

    Renderer& Render(const Texture &arTexture) override;

    void Present() override;
};

} /* namespace rsp::graphics::sw */

#endif /* INCLUDE_GRAPHICS_SW_SWRENDERER_H_ */
