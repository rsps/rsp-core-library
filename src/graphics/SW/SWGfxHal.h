/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifndef RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_GFX_HAL_H
#define RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_GFX_HAL_H

#ifdef USE_GFX_SW

#include <utils/Singleton.h>
#include "GfxHal.h"

namespace rsp::graphics::sw {

class SWGfxHal: public GfxHal, public rsp::utils::Singleton<SWGfxHal>
{
public:
    std::shared_ptr<VideoSurface> Alloc(int aWidth, int aHeight) override;
    [[nodiscard]] uint64_t GetVideoMemoryUsage() const override;
    void Blit(VideoSurface &arDst, const VideoSurface &arSrc, OptionalRect aDstRect, OptionalRect aSrcRect) override;
    void CopyFrom(VideoSurface &arDst, const PixelData &arPixelData, uint32_t aColor, OptionalRect aDstRect, OptionalRect aSrcRect) override;
    void DrawRect(VideoSurface &arDst, uint32_t aColor, const Rect &arRect) override;
    void Fill(VideoSurface &arDst, uint32_t aColor, OptionalRect aDest) override;
    void Sync() override;
    [[nodiscard]] uint32_t GetPixel(const VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY) const override;
    void SetPixel(VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, uint32_t aColor) override;

    static uint64_t mAllocated;
};

} /* namespace rsp::graphics */

#endif /* USE_GFX_SW */

#endif // RSP_CORE_LIB_SRC_GRAPHICS_SW_SW_GFX_HAL_H
