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
    SWTexture(GuiUnit_t aWidth, GuiUnit_t aHeight);

    void Fill(Color aColor, GfxHal::Optional<const Rect> arRect = nullptr) override;
    void Update(const PixelData &arPixelData, Color aColor) override;

protected:
    GfxHal &mrGfxHal;
    VideoSurface mSurface{};

    friend SWRenderer;
    const VideoSurface& getSurface() const { return mSurface; }
};

} /* namespace rsp::graphics::sw */

#endif /* INCLUDE_GRAPHICS_SW_SWTEXTURE_H_ */
