/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_RENDERER_H
#define RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_RENDERER_H

#ifdef USE_GFX_SW

#include <string>
#include <vector>
#include <graphics/Renderer.h>
#include "TouchParser.h"
#include <utils/Singleton.h>
#include "Framebuffer.h"

namespace rsp::graphics::sw {

class SWRenderer: public Renderer, public Framebuffer, public rsp::utils::Singleton<SWRenderer>
{
public:
    SWRenderer(GuiUnit_t aWidth, GuiUnit_t aHeight);

    GuiUnit_t GetWidth() const override;
    GuiUnit_t GetHeight() const override;
    ColorDepth GetColorDepth() const override;

    Renderer& DrawRect(const Color &arColor, const Rect &arRect) override;
    Renderer& Fill(const Color &arColor, OptionalRect aDestination) override;
    Renderer& Blit(const Texture &arTexture) override;
    Renderer& PushClipRect(const Rect &arClipRect) override;
    Renderer& PopClipRect() override;
    Renderer& ClearClipRect() override;
    Renderer& Flush() override;
    void Present() override;

    Renderer& SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) override;
    Color GetPixel(GuiUnit_t aX, GuiUnit_t aY) const override;

protected:
    Rect mClipRect;
    std::vector<Rect> mClipRectList{};
    TouchParser mTouchParser{};
};

} /* namespace rsp::graphics::sw */

#endif /* USE_GFX_SW */
#endif // RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_RENDERER_H
