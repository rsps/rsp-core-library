/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_PRIMITIVES_RENDERER_H_
#define INCLUDE_GRAPHICS_PRIMITIVES_RENDERER_H_

#include <memory>
#include "Color.h"
#include "GuiUnit.h"
#include "PixelData.h"
#include "Rect.h"
#include "Texture.h"
#include <graphics/TouchEvent.h>

namespace rsp::graphics {


class Renderer
{
public:
    static Renderer& Make();

    virtual ~Renderer() {}

    virtual GuiUnit_t GetHeight() const = 0;
    virtual GuiUnit_t GetWidth() const = 0;
    virtual PixelData::ColorDepth GetColorDepth() const = 0;

    virtual std::unique_ptr<Texture> CreateTexture(GuiUnit_t aWidth, GuiUnit_t aHeight) = 0;
    virtual std::unique_ptr<Texture> CreateStaticTexture(const PixelData &arPixelData) = 0;

    virtual Renderer& RenderTexture(const Texture &arTexture, const Rect &arDestination) = 0;

    virtual Renderer& DrawRect(const Rect &arRect, Color aColor) = 0;
    virtual Renderer& Fill(Color aColor) = 0;

    virtual void Present() = 0;

    virtual bool PollEvents(TouchEvent &arTouchEvent) = 0;
    virtual Renderer& FlushEvents() = 0;

protected:
    Renderer() {};
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_RENDERER_H_ */
