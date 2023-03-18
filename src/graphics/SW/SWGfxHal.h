/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef SRC_GRAPHICS_SW_SWGFXHAL_H_
#define SRC_GRAPHICS_SW_SWGFXHAL_H_

#include <graphics/GfxHal.h>
#include <utils/Singleton.h>

#ifdef USE_GFX_SW

namespace rsp::graphics::sw {

class SWGfxHal: public rsp::graphics::GfxHal, public rsp::utils::Singleton<SWGfxHal>
{
public:
    std::shared_ptr<VideoSurface> Alloc(int aWidth, int aHeight) override;
    void Blit(VideoSurface &arDst, const VideoSurface &arSrc, OptionalRect aDstRect, OptionalRect aSrcRect) override;
    void CopyFrom(VideoSurface &arDst, const PixelData &arPixelData, uint32_t aColor, OptionalRect aDstRect, OptionalRect aSrcRect) override;
    void DrawRect(VideoSurface &arDst, uint32_t aColor, const Rect &arRect) override;
    void Fill(VideoSurface &arDst, uint32_t aColor, OptionalRect aDest) override;
    void Sync() override;
    uint32_t GetPixel(const VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, bool aFrontBuffer) const override;
    void SetPixel(VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, uint32_t aColor) override;
};

} /* namespace rsp::graphics */

#endif /* USE_GFX_SW */

#endif /* SRC_GRAPHICS_SW_SWGFXHAL_H_ */
