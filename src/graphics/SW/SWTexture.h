/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SW_SWTEXTURE_H_
#define SRC_GRAPHICS_SW_SWTEXTURE_H_

#include <graphics/GfxHal.h>
#include <graphics/PixelData.h>
#include <graphics/Texture.h>

namespace rsp::graphics::sw {

class SWRenderer;


class SWTexture: public rsp::graphics::Texture
{
public:
    SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos, const Point &arDestOffset);

    GuiUnit_t GetWidth() const override;
    GuiUnit_t GetHeight() const override;

    Texture& Fill(Color aColor, GfxHal::OptionalRect arRect = nullptr) override;
    Texture& Update(const PixelData &arPixelData, Color aColor) override;
    Texture& SetBlendOperation(GfxBlendOperation aOp, Color aColorKey = Color::None) override;
    Texture& SetSourceRect(const Rect &arRect) override;
    Texture& SetDestination(const Point &arPoint) override;
    Point GetDestination() const override;
    Texture& SetOffset(const Point &arPoint) override;
    std::unique_ptr<Texture> Clone() const override;
    Rect GetDestinationRect() const override;
    const Rect& GetSourceRect() const override { return mSourceRect; }

    rsp::graphics::Texture& Blit(const rsp::graphics::Texture &arTexture) override;
    rsp::graphics::Texture& DrawRect(rsp::graphics::Color aColor, const rsp::graphics::Rect &arRect) override;

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

#endif /* SRC_GRAPHICS_SW_SWTEXTURE_H_ */
