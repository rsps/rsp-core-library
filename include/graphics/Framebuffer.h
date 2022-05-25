/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */
#ifndef FRAMEBUFFERCANVAS_H
#define FRAMEBUFFERCANVAS_H

#include <linux/fb.h>

#include "graphics/BufferedCanvas.h"
#include "graphics/primitives/Canvas.h"

namespace rsp::graphics
{

class Framebuffer : public BufferedCanvas
{
  public:
    Framebuffer(const char *apDevPath = nullptr);
    virtual ~Framebuffer();

    /**
     * \brief Sets a single pixel to the given Color
     * \param aPoint Reference to the coordinate for the pixel to be set
     * \param aColor Reference to the color the pixel is set to
     */
    inline void SetPixel(const Point &arPoint, const Color &arColor) override
    {
        if (!IsInsideScreen(arPoint)) {
            return;
        }
        std::uint32_t location = ((static_cast<std::uint32_t>(arPoint.mX) + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8)
            + static_cast<std::uint32_t>(arPoint.mY) * mFixedInfo.line_length) / sizeof(std::uint32_t);
        mpBackBuffer[location] = arColor;
    }

    /**
     * \brief Gets a single pixel to the given Color
     * \param aPoint Reference to the coordinate for the pixel to get
     * \param aFront Gets pixels from the backbuffer by default, set true to read front buffer
     */
    uint32_t GetPixel(const Point &aPoint, const bool aFront = false) const;

    /**
     * \brief Swaps front and back buffers
     * \param aSwapOp The type of swap operations to be executed, default is copy
     * \param aColor In case of Clear operation a color is needed, default is black
     */
    void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy, Color aColor = Color::Black);

  protected:
    int mFramebufferFile;
    int mTtyFb = 0;
    struct fb_fix_screeninfo mFixedInfo {
    };
    struct fb_var_screeninfo mVariableInfo {
    };

    void clear(Color aColor);
    void copy();
};

} // namespace rsp::graphics
#endif // FRAMEBUFFERCANVAS_H
