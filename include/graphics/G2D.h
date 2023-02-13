/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright (c) 2023 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      steffen
 */

#ifndef INCLUDE_GRAPHICS_G2D_H_
#define INCLUDE_GRAPHICS_G2D_H_

#include <graphics/Framebuffer.h>
#include <graphics/imx_g2d.h>
#include <utils/CoreException.h>

namespace rsp::graphics {

class G2DException: public rsp::utils::CoreException
{
public:
    explicit G2DException(const char *aMsg)
        : CoreException(aMsg)
    {
    }
};


class G2D: public Framebuffer
{
public:
    G2D(const char *apDevPath = nullptr);
    virtual ~G2D();

    G2D(const G2D&) = delete;
    G2D(G2D&&) = delete;
    G2D& operator =(const G2D&) = delete;
    G2D& operator =(G2D&&) = delete;

    void SwapBuffer(BufferedCanvas::SwapOperations aSwapOp, Color aColor) override;

    void DrawRectangle(const Rect &arRect, const Color &arColor, bool aFilled) override;
    void DrawPixelData(const Point &arLeftTop, const PixelData &arPixelData, const Rect &arSection, Color aColor) override;
    void DrawLine(const Point &arA, const Point &arB, const Color &arColor) override;
    void DrawText(const Text &arText, Color aColor, Color aBackColor) override;

    uint32_t GetPixel(const Point &arPoint, bool aFront) const override;
    void SetPixel(const Point &arPoint, const Color &arColor) override;

protected:
    void *mpHandle = nullptr;
    g2d_surface mScreen[2]{};
    int mCurrentSurface = 0;

    void clear(Color aColor) override;
    void copy() override;
};

} /* namespace rsp::graphics */

#endif /* INCLUDE_GRAPHICS_G2D_H_ */
