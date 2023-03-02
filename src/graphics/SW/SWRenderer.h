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

#include <string>
#include <graphics/Renderer.h>
#include <graphics/Framebuffer.h>
#include <graphics/TouchParser.h>

namespace rsp::graphics {

class SWRenderer: public Renderer, public Framebuffer
{
public:
    SWRenderer(const std::string &arRenderDevice, const std::string &arTouchDevice);

    GuiUnit_t GetWidth() const override;
    Renderer& DrawRect(const Rect &arRect, Color aColor) override;
    Renderer& Fill(Color aColor) override;
    Renderer& FlushEvents() override;
    std::unique_ptr<Texture,std::default_delete<Texture> > CreateTexture(GuiUnit_t aWidth,
        GuiUnit_t aHeight) override;
    std::unique_ptr<Texture,std::default_delete<Texture> > CreateStaticTexture(
        const PixelData &arPixelData) override;
    Renderer& RenderTexture(const Texture &arTexture, const Rect &arDestination) override;
    void Present() override;
    GuiUnit_t GetHeight() const override;
    PixelData::ColorDepth GetColorDepth() const override;
    bool PollEvents(TouchEvent &arTouchEvent) override;

protected:
    TouchParser mTouchParser;
};

} /* namespace rsp::graphics */

#endif /* SRC_GRAPHICS_SW_SWRENDERER_H_ */
