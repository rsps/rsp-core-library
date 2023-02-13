/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */

#include <graphics/G2D.h>

namespace rsp::graphics {

G2D::G2D(const char *apDevPath)
    : Framebuffer(apDevPath)
{
    if (g2d_open(&mpHandle) < 0) {
        THROW_WITH_BACKTRACE1(G2DException, "g2d_open failed");
    }

    mScreen[0].format = G2D_RGBA8888;
    mScreen[0].planes[0] = reinterpret_cast<uintptr_t>(mpFrontBuffer);
    mScreen[0].left = 0;
    mScreen[0].top = 0;
    mScreen[0].right = static_cast<int>(mFixedInfo.line_length);
    mScreen[0].bottom = static_cast<int>(mVariableInfo.yres);
    mScreen[0].width = mWidth;
    mScreen[0].height = mHeight;
    mScreen[0].blendfunc = G2D_ONE;
    mScreen[0].global_alpha = 255;
    mScreen[0].clrcolor = 0;
    mScreen[0].rot = G2D_ROTATION_0;

    mScreen[1] = mScreen[0];
    mScreen[1].planes[0] = reinterpret_cast<uintptr_t>(mpBackBuffer);
}

G2D::~G2D()
{
    if (mpHandle) {
        g2d_close(mpHandle);
    }
}

void G2D::SwapBuffer(BufferedCanvas::SwapOperations aSwapOp, Color aColor)
{
    g2d_finish(mpHandle);
    Framebuffer::SwapBuffer(aSwapOp, aColor);
    mCurrentSurface = (mCurrentSurface + 1) % 2;
}

void G2D::clear(Color aColor)
{
    mScreen[mCurrentSurface].clrcolor = static_cast<int>(aColor.AsUint());
    if (g2d_clear(mpHandle, &mScreen[mCurrentSurface]) < 0) {
        THROW_WITH_BACKTRACE1(G2DException, "g2d_clear failed");
    }
}

void G2D::copy()
{
    if (g2d_blit(mpHandle, &mScreen[mCurrentSurface], &mScreen[(mCurrentSurface+1)%2]) < 0) {
        THROW_WITH_BACKTRACE1(G2DException, "g2d_blit failed in copy");
    }
}

void G2D::DrawRectangle(const Rect &arRect, const Color &arColor, bool aFilled)
{
    if (aFilled) {
        g2d_surface srf = mScreen[mCurrentSurface];
        srf.left = arRect.GetLeft();
        srf.right = arRect.GetRight();
        srf.top = arRect.GetTop();
        srf.bottom = arRect.GetBottom();
        srf.clrcolor = static_cast<int>(arColor.AsUint());;
        if (g2d_clear(mpHandle, &srf) < 0) {
            THROW_WITH_BACKTRACE1(G2DException, "g2d_clear failed in DrawRectangle");
        }
    }
    else {
        Canvas::DrawRectangle(arRect, arColor, aFilled);
    }
}

void G2D::DrawPixelData(const Point &arLeftTop, const PixelData &arPixelData, const Rect &arSection, Color aColor)
{
    g2d_surface src;

    switch (arPixelData.GetColorDepth()) {
        case PixelData::ColorDepth::Monochrome:
            Canvas::DrawPixelData(arLeftTop, arPixelData, arSection, aColor);
            return;

        case PixelData::ColorDepth::Alpha:
            Canvas::DrawPixelData(arLeftTop, arPixelData, arSection, aColor);
            return;

        case PixelData::ColorDepth::RGB:
            src.format = G2D_RGBX8888;
            break;

        case PixelData::ColorDepth::RGBA:
            src.format = G2D_RGBA8888;
            break;

        default:
            THROW_WITH_BACKTRACE(EIllegalColorDepth);
            break;
    }

    src.planes[0] = reinterpret_cast<uintptr_t>(arPixelData.GetData());
    src.left = arSection.GetLeft();
    src.top = arSection.GetTop();
    src.right = arSection.GetRight();
    src.bottom = arSection.GetBottom();
    src.width = arPixelData.GetWidth();
    src.height = arPixelData.GetHeight();
    src.blendfunc = G2D_SRC_ALPHA;
    src.global_alpha = 255;
    src.clrcolor = 0;
    src.rot = G2D_ROTATION_0;

    if (g2d_blit(mpHandle, &src, &mScreen[mCurrentSurface]) < 0) {
        THROW_WITH_BACKTRACE1(G2DException, "g2d_blit failed in DrawPixelData");
    }
}

void G2D::DrawLine(const Point &arA, const Point &arB, const Color &arColor)
{
    Canvas::DrawLine(arA, arB, arColor);
}

void G2D::DrawText(const Text &arText, Color aColor, Color aBackColor)
{
    Canvas::DrawText(arText, aColor, aBackColor);
}

} /* namespace rsp::graphics */
