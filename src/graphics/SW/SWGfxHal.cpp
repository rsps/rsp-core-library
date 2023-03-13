/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Steffen Brummer
 */

#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "SWGfxHal.h"

#ifdef USE_GFX_SW

namespace rsp::graphics {

GfxHal& GfxHal::Get()
{
    if (!sw::SWGfxHal::HasInstance()) {
        sw::SWGfxHal::Create();
    }

    return rsp::utils::Singleton<sw::SWGfxHal>::Get();
}

} /* namespace rsp::graphics */

namespace rsp::graphics::sw {


void SWGfxHal::Alloc(VideoSurface &arSurface, int aWidth, int aHeight)
{
    arSurface.mpPhysAddr = std::unique_ptr<uint32_t[]>(new uint32_t[aWidth * aHeight]);
    arSurface.mRowPitch = uintptr_t(aWidth) * sizeof(uint32_t);
    arSurface.mWidth = aWidth;
    arSurface.mHeight = aHeight;
}

/**
 * \brief Same blending as in Color::Blend
 */
static inline uint32_t alphaBlend(uint32_t bg, uint32_t fg)
{
    uint32_t a = fg >> 24;

    /* If source pixel is transparent, just return the background */
    if (0 == a) {
      return bg;
    }

    if (255 == a) {
        return fg;
    }

    uint32_t rb = ((((fg & 0x00ff00ff) * a) + 0x00007F007F) + ((bg & 0x00ff00ff) * (0xff - a))) & 0xff00ff00;
    uint32_t g  = ((((fg & 0x0000ff00) * a) + 0x00007f00) + ((bg & 0x0000ff00) * (0xff - a))) & 0x00ff0000;
    uint32_t result = 0xff000000 | ((rb | g) >> 8);

    return result;
}

constexpr uint32_t* offset(uint32_t *addr, uintptr_t pitch, uintptr_t x, uintptr_t y) {
    return reinterpret_cast<uint32_t*>(uintptr_t(addr) + (y * pitch) + (x * sizeof(uint32_t)));
}

constexpr uint32_t* offset(uint32_t *addr, uintptr_t pitch, GuiUnit_t x, GuiUnit_t y) {
    return offset(addr, pitch, uintptr_t(x), uintptr_t(y));
}

static inline void paintPixel(GfxBlendOperation op, uint32_t *dst, uint32_t aColor)
{
    switch(op) {
        default:
        case GfxBlendOperation::Copy: {
            *dst = aColor;
            break;
        }

        case GfxBlendOperation::SourceAlpha: {
            *dst = alphaBlend(*dst, aColor);
            break;
        }

        case GfxBlendOperation::ColorKey: {
            *dst = aColor;
            break;
        }
    }
}

void SWGfxHal::Blit(VideoSurface &arDst, const VideoSurface &arSrc, Optional<const Rect> aDstRect, Optional<const Rect> aSrcRect)
{
    Rect dr(0, 0, arDst.mWidth, arDst.mHeight);
    Rect sr(0, 0, arSrc.mWidth, arSrc.mHeight);

    if (aDstRect) {
        dr &= *aDstRect;
    }
    if (aSrcRect) {
        sr &= *aSrcRect;
    }

    uint32_t *src = arSrc.mpPhysAddr.get();
    uint32_t *dst = arDst.mpPhysAddr.get();

    GuiUnit_t y_end = std::min(dr.GetHeight(), sr.GetHeight()) + dr.GetTop();
    size_t w = size_t(std::min(dr.GetWidth(), sr.GetWidth()));
    GuiUnit_t src_y = sr.GetTop();

    switch(arDst.mBlendOperation) {
        default:
        case GfxBlendOperation::Copy: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                std::memcpy(offset(dst, arDst.mRowPitch, dr.GetLeft(), y), offset(src, arSrc.mRowPitch, sr.GetLeft(), src_y++), w * sizeof(uint32_t));
            }
            break;
        }

        case GfxBlendOperation::SourceAlpha: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, arDst.mRowPitch, dr.GetLeft(), y);
                uint32_t *source = offset(src, arSrc.mRowPitch, sr.GetLeft(), src_y++);
                for(size_t x = 0; x < w ; ++x) {
                    dest[x] = alphaBlend(dest[x], source[x]);
                }
            }
            break;
        }

        case GfxBlendOperation::ColorKey: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, arDst.mRowPitch, dr.GetLeft(), y);
                uint32_t *source = offset(src, arSrc.mRowPitch, sr.GetLeft(), src_y++);
                for(size_t x = 0; x < w ; ++x) {
                    if (source[x] != arDst.mColorKey) {
                        dest[x] = source[x];
                    }
                }
            }
            break;
        }
    }
}

void SWGfxHal::CopyFrom(VideoSurface &arDst, const PixelData &arPixelData, uint32_t aColor, Optional<const Rect> aDstRect, Optional<const Rect> aSrcRect)
{
    Rect dr(0, 0, arDst.mWidth, arDst.mHeight);
    Rect sr(arPixelData.GetRect());

    if (aDstRect) {
        dr &= *aDstRect;
    }
    if (aSrcRect) {
        sr &= *aSrcRect;
    }

    uint32_t *dst = arDst.mpPhysAddr.get();

    GuiUnit_t y_end = uintptr_t(std::min(dr.GetHeight(), sr.GetHeight()));
    size_t w = size_t(std::min(dr.GetWidth(), sr.GetWidth()));
    GuiUnit_t src_y = sr.GetTop();

    switch(arDst.mBlendOperation) {
        default:
        case GfxBlendOperation::Copy: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, arDst.mRowPitch, dr.GetLeft(), y);
                GuiUnit_t src_x = sr.GetLeft();
                for(size_t x = 0; x < w ; ++x) {
                    dest[x] = arPixelData.GetPixelAt(src_x++, src_y, aColor);
                }
                ++src_y;
            }
            break;
        }

        case GfxBlendOperation::SourceAlpha: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, arDst.mRowPitch, dr.GetLeft(), y);
                GuiUnit_t src_x = sr.GetLeft();
                for(size_t x = 0; x < w ; ++x) {
                    dest[x] = alphaBlend(dest[x], arPixelData.GetPixelAt(src_x++, src_y, aColor));
                }
                ++src_y;
            }
            break;
        }

        case GfxBlendOperation::ColorKey: {
            for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
                uint32_t *dest = offset(dst, arDst.mRowPitch, dr.GetLeft(), y);
                GuiUnit_t src_x = sr.GetLeft();
                for(size_t x = 0; x < w ; ++x) {
                    uint32_t cl = arPixelData.GetPixelAt(src_x++, src_y, aColor);
                    if (cl != arDst.mColorKey) {
                        dest[x] = cl;
                    }
                }
                ++src_y;
            }
            break;
        }
    }
}

void SWGfxHal::DrawRect(VideoSurface &arDst, uint32_t aColor, const Rect &arRect)
{
    uintptr_t pitch = arDst.mRowPitch;

    auto left = std::max(arRect.GetLeft()+1, 0);
    auto right = std::min(arRect.GetRight()-1, arDst.mWidth);
    auto top = arRect.GetTop();
    auto bottom = arRect.GetBottom()-1;

    if (arDst.mBlendOperation == GfxBlendOperation::ColorKey) {
        if (aColor == arDst.mColorKey) {
            return;
        }
    }

    if (left < arDst.mWidth && right > 0) {
        // Top horizontal
        if (top >= 0 && top < arDst.mHeight) {
            auto *lp = offset(arDst.mpPhysAddr.get(), pitch, left, top);
            for (GuiUnit_t i = left; i < right ; ++i) {
                paintPixel(arDst.mBlendOperation, lp, aColor);
                lp++;
            }
        }

        // Bottom horizontal
        if (bottom >= 0 && bottom < arDst.mHeight && left < arDst.mWidth && right > 0) {
            auto *lp = offset(arDst.mpPhysAddr.get(), pitch, left, bottom);
            for (GuiUnit_t i = left; i < right ; ++i) {
                paintPixel(arDst.mBlendOperation, lp, aColor);
                lp++;
            }
        }
    }

    top = std::max(arRect.GetTop(), 0);
    bottom = std::min(arRect.GetBottom(), arDst.mHeight);

    if (top < arDst.mHeight && bottom > 0) {
        // Left vertical
        left = arRect.GetLeft();
        if (left >= 0 && left < arDst.mWidth) {
            auto *lp = offset(arDst.mpPhysAddr.get(), pitch, left, top);
            for (GuiUnit_t i = top; i < bottom ; ++i) {
                paintPixel(arDst.mBlendOperation, lp, aColor);
                lp = reinterpret_cast<uint32_t*>(uintptr_t(lp) + pitch);
            }
        }

        // Right Vertical
        right = arRect.GetRight() - 1;
        if (right >= 0 && right < arDst.mWidth) {
            auto *lp = offset(arDst.mpPhysAddr.get(), pitch, right, top);
            for (GuiUnit_t i = top; i < bottom ; ++i) {
                paintPixel(arDst.mBlendOperation, lp, aColor);
                lp = reinterpret_cast<uint32_t*>(uintptr_t(lp) + pitch);
            }
        }
    }
}

void SWGfxHal::Fill(VideoSurface &arDst, uint32_t aColor, Optional<const Rect> aDstRect)
{
    Rect dr(0, 0, arDst.mWidth, arDst.mHeight);
    if (aDstRect) {
        dr &= *aDstRect;
    }

    if (arDst.mBlendOperation == GfxBlendOperation::ColorKey) {
        if (aColor == arDst.mColorKey) {
            return;
        }
    }

    GuiUnit_t y_end = dr.GetBottom();
    GuiUnit_t x_end = dr.GetRight();

    for (GuiUnit_t y = dr.GetTop(); y < y_end ; ++y) {
        uint32_t *dst = offset(arDst.mpPhysAddr.get(), arDst.mRowPitch, dr.GetLeft(), y);
        for (GuiUnit_t x = dr.GetLeft() ; x < x_end ; ++x) {
            switch(arDst.mBlendOperation) {
                default:
                case GfxBlendOperation::Copy: {
                    *dst = aColor;
                    break;
                }

                case GfxBlendOperation::SourceAlpha: {
                    *dst = alphaBlend(*dst, aColor);
                    break;
                }

                case GfxBlendOperation::ColorKey: {
                    *dst = aColor;
                    break;
                }
            }
            ++dst;
        }
    }
}

void SWGfxHal::Sync()
{
    // The software operations perform immediately so we do nothing here.
}

uint32_t SWGfxHal::GetPixel(const VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, bool aFrontBuffer) const
{
    if (aX >= 0 && aX < arSurface.mWidth && aY >= 0 && aY < arSurface.mHeight) {
        return *offset(arSurface.mpPhysAddr.get(), arSurface.mRowPitch, aX, aY);
    }
    return 0;
}

void SWGfxHal::SetPixel(VideoSurface &arSurface, GuiUnit_t aX, GuiUnit_t aY, uint32_t aColor)
{
    if (aX >= 0 && aX < arSurface.mWidth && aY >= 0 && aY < arSurface.mHeight) {
        uint32_t *dst = offset(arSurface.mpPhysAddr.get(), arSurface.mRowPitch, aX, aY);
        paintPixel(arSurface.mBlendOperation, dst, aColor);
    }
}

} /* namespace rsp::graphics */

#endif /* USE_GFX_SW */
