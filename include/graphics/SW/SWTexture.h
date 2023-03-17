/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef INCLUDE_GRAPHICS_SW_SWTEXTURE_H_
#define INCLUDE_GRAPHICS_SW_SWTEXTURE_H_

#include <graphics/GfxHal.h>
#include <graphics/PixelData.h>
#include <graphics/Texture.h>

namespace rsp::graphics::sw {

class SWRenderer;


class SWTexture: public rsp::graphics::Texture
{
public:
    SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight, const Point &arDestPos);

    GuiUnit_t GetWidth() const override;
    GuiUnit_t GetHeight() const override;

    Texture& Fill(Color aColor, GfxHal::Optional<const Rect> arRect = nullptr) override;
    Texture& Update(const PixelData &arPixelData, Color aColor) override;
    Texture& SetBlendOperation(GfxBlendOperation aOp, Color aColorKey = Color::None) override;
    Texture& SetSourceRect(const Rect &arRect) override;
    Texture& SetDestination(const Point &arPoint) override;
    std::unique_ptr<Texture> Clone() const override;
    const Rect& GetDestinationRect() const override { return mDestinationRect; }
    const Rect& GetSourceRect() const override { return mSourceRect; }

protected:
    SWTexture(const SWTexture& arOther) = default;

    GfxHal &mrGfxHal;
    friend SWRenderer;
    std::shared_ptr<VideoSurface> mpSurface{};
    Rect mSourceRect;
    Rect mDestinationRect;
};

} /* namespace rsp::graphics::sw */

#endif /* INCLUDE_GRAPHICS_SW_SWTEXTURE_H_ */
