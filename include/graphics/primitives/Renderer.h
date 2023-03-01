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
#include "Point.h"
#include "PixelData.h"
#include "Rect.h"
#include "Texture.h"

namespace rsp::graphics {


class Renderer
{
public:
    Renderer();
    virtual ~Renderer() {}

    virtual GuiUnit_t GetHeight() const
    {
        return mpImpl->GetHeight();
    }

    virtual GuiUnit_t GetWidth() const
    {
        return mpImpl->GetWidth();
    }

    virtual PixelData::ColorDepth GetColorDepth() const
    {
        return mpImpl->GetColorDepth();
    }

    virtual void RenderTexture(const Texture &arTexture, const Rect &arDestination)
    {
        mpImpl->RenderTexture(arTexture, arDestination);
    }

    virtual void DrawRect(const Rect &arTouchArea, Color aTouchAreaColor)
    {
        mpImpl->DrawRect(arTouchArea, aTouchAreaColor);
    }

protected:
    std::unique_ptr<Renderer> mpImpl;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_PRIMITIVES_RENDERER_H_ */
