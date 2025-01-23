/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_TEXTURE_H
#define RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_TEXTURE_H

#ifdef USE_GFX_SW

#include <graphics/PixelData.h>
#include <graphics/Texture.h>
#include "GfxHal.h"

namespace rsp::graphics::sw {

class SWRenderer;


class SWTexture: public rsp::graphics::Texture
{
public:
    SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset);

    [[nodiscard]] GuiUnit_t GetWidth() const override;
    [[nodiscard]] GuiUnit_t GetHeight() const override;

    Texture& Fill(const Color &arColor, GfxHal::OptionalRect) override;
    Texture& Update(const PixelData &arPixelData, const Color &arColor) override;
    Texture& SetBlendOperation(Texture::BlendOperation aOp, const Color &arColorKey) override;
    Texture& SetSourceRect(const Rect &arRect) override;
    Texture& SetDestination(const Point &arPoint) override;
    [[nodiscard]] Point GetDestination() const override;
    Texture& SetOffset(const Point &arPoint) override;
    [[nodiscard]] std::unique_ptr<Texture> Clone() const override;
    [[nodiscard]] Rect GetDestinationRect() const override;
    [[nodiscard]] const Rect& GetSourceRect() const override;

protected:
    SWTexture(const SWTexture& arOther) = default;

    GfxHal &mrGfxHal;
    friend SWRenderer;
    std::shared_ptr<VideoSurface> mpSurface{};
    Rect mSourceRect;
    Point mDestinationPos;
    Point mDestinationOffset;
};

} /* namespace rsp::graphics::sw */

#endif /* USE_GFX_SW */
#endif // RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_TEXTURE_H
