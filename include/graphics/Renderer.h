/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_RENDERER_H_
#define INCLUDE_GRAPHICS_RENDERER_H_

#include <graphics/Color.h>
#include <graphics/GuiUnit.h>
#include <graphics/PixelData.h>
#include <graphics/Rect.h>
#include <graphics/Texture.h>
#include <utils/OptionalPtr.h>
#include <functional>
#include <memory>
#include <optional>

namespace rsp::graphics {

class Renderer
{
public:
    template <class T>
    using Optional = rsp::utils::OptionalPtr<T>;

    static Renderer& Get();

    virtual ~Renderer() {}

    virtual GuiUnit_t GetHeight() const = 0;
    virtual GuiUnit_t GetWidth() const = 0;
    virtual PixelData::ColorDepth GetColorDepth() const = 0;

    virtual std::shared_ptr<Texture> CreateTexture(GuiUnit_t aWidth = 0, GuiUnit_t aHeight = 0) = 0;
    virtual std::shared_ptr<const Texture> CreateStaticTexture(const PixelData &arPixelData) = 0;

    virtual Renderer& Render(const Texture &arTexture, Optional<const Rect> aDestination = nullptr, Optional<const Rect> aSource = nullptr) = 0;
    virtual Renderer& RenderTo(const Texture &arTexture, const Point &arPosition, Optional<const Rect> aSource = nullptr) = 0;

    virtual Renderer& DrawRect(Color aColor, const Rect &arRect) = 0;
    virtual Renderer& Fill(Color aColor, Optional<const Rect> aDestination = nullptr) = 0;

    virtual void Present() = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_RENDERER_H_ */
