/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#ifdef USE_GFX_SW

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "SWGfxHal.h"

namespace rsp::graphics {

GfxHal& GfxHal::Get()
{
    static SWGfxHal instance;
    return instance;
}


void SWGfxHal::Alloc(VideoSurface &arSurface, int aWidth, int aHeight)
{
    arSurface.mpPhysAddr = std::unique_ptr<uint32_t>(new uint32_t[aWidth * aHeight], [](uint32_t *p){ delete[] p; });
    arSurface.mRowPitch = aWidth;
    arSurface.mWidth = aWidth;
    arSurface.mHeight = aHeight;
}


void SWGfxHal::Blit(VideoSurface &arDst, const VideoSurface &arSrc, OptionalPtr<Rect> aDstRect, OptionalPtr<Rect> aSrcRect)
{
    Rect dr(0, 0, arDst.mWidth, arDst.mHeight);
    Rect sr(0, 0, arSrc.mWidth, arSrc.mHeight);

    if (aDstRect) {
        dr &= *aDstRect;
    }
    if (aSrcRect) {
        sr &= *aSrcRect;
    }

    uint32_t *src = arSrc.mpPhysAddr.get() + sr.GetLeft();
    uint32_t *dst = arDst.mpPhysAddr.get() + dr.GetLeft();

    int y_end = std::min(dr.GetHeight(), sr.GetHeight());
    int w = std::min(dr.GetWidth(), sr.GetWidth());

    for (int y = dr.GetTop(); y < y_end ; ++y) {
        std::memcpy((dst + (y * GuiUnit_t(arDst.mRowPitch))), (src + (y * GuiUnit_t(arSrc.mRowPitch))), size_t(w));
    }
}

void SWGfxHal::DrawRect(VideoSurface &arDst, const Rect &arRect, uint32_t aColor)
{
    Rect r = arRect & Rect(0, 0, arDst.mWidth, arDst.mHeight);

    GuiUnit_t pitch = GuiUnit_t(arDst.mRowPitch);

    uint32_t *left = arDst.mpPhysAddr.get() + (((r.GetTop() * pitch) + r.GetLeft()) * sizeof(uint32_t));
    uint32_t *right = arDst.mpPhysAddr.get() + (((r.GetTop() * pitch) + r.GetRight() - 1) * sizeof(uint32_t));
    for (int y = r.GetTop() ; y < r.GetBottom() ; ++y) {
        *left = aColor;
        *right = aColor;
        left += arDst.mRowPitch;
        right += arDst.mRowPitch;
    }

    uint32_t *top = arDst.mpPhysAddr.get() + (arRect.GetTop() * pitch);
    uint32_t *bot = arDst.mpPhysAddr.get() + ((arRect.GetBottom() - 1) * pitch);
    for (int x = r.GetLeft() ; x < r.GetRight() ; ++x) {
        *top = aColor;
        *bot = aColor;
        ++top;
        ++bot;
    }
}

void SWGfxHal::Fill(VideoSurface &arDst, uint32_t aColor, OptionalPtr<Rect> aDstRect)
{
    Rect dr(0, 0, arDst.mWidth, arDst.mHeight);
    if (aDstRect) {
        dr &= *aDstRect;
    }

    int y_end = dr.GetBottom();
    int x_end = dr.GetRight();

    for (int y = dr.GetTop(); y < y_end ; ++y) {

        uint32_t *dst = arDst.mpPhysAddr.get() + (y * GuiUnit_t(arDst.mRowPitch));

        for (int x = dr.GetLeft() ; x < x_end ; ++x) {
            *dst = aColor;
            ++dst;
        }
    }
}

void SWGfxHal::Sync()
{
    // The software operations perform immediately so we do nothing here.
}

} /* namespace rsp::graphics */

#endif /* USE_GFX_SW */
