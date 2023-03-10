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
    void Alloc(VideoSurface &arSurface, int aWidth, int aHeight) override;
    void Blit(VideoSurface &arDst, const VideoSurface &arSrc, OptionalPtr<Rect> aDstRect, OptionalPtr<Rect> aSrcRect) override;
    void DrawRect(VideoSurface &arDst, uint32_t aColor, const Rect &arRect) override;
    void Fill(VideoSurface &arDst, uint32_t aColor, OptionalPtr<Rect> aDest) override;
    void SetBlendOperation(GfxBlendOperation aOp) override { mBlendOperation = aOp; }
    void SetColorKey(uint32_t aColor) override { mColorKey = (aColor & 0x00FFFFFF); }
    void Sync() override;

protected:
    GfxBlendOperation mBlendOperation = GfxBlendOperation::Copy;
    uint32_t mColorKey = 0;
};

} /* namespace rsp::graphics */

#endif /* USE_GFX_SW */

#endif /* SRC_GRAPHICS_SW_SWGFXHAL_H_ */
