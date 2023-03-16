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

#include <functional>
#include <memory>
#include <optional>
#include <utils/OptionalPtr.h>
#include "Color.h"
#include "GuiUnit.h"
#include "PixelData.h"
#include "Rect.h"
#include "Texture.h"

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

    virtual Renderer& DrawRect(Color aColor, const Rect &arRect) = 0;
    virtual Renderer& Fill(Color aColor, Optional<const Rect> aDestination = nullptr) = 0;
    virtual Renderer& Render(const Texture &arTexture) = 0;

    virtual void Present() = 0;

    virtual Renderer& SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor) = 0;
    virtual uint32_t GetPixel(GuiUnit_t aX, GuiUnit_t aY, bool aFront = false) const = 0;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_RENDERER_H_ */
