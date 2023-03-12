/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <graphics/Rect.h>
#include <graphics/GfxHal.h>
#include <linux/fb.h>


namespace rsp::graphics
{

class Framebuffer
{
public:
    static const char *mpDevicePath;

    Framebuffer();
    virtual ~Framebuffer();

    Framebuffer(const Framebuffer&) = default;
    Framebuffer(Framebuffer&&) = default;
    Framebuffer& operator=(const Framebuffer&) = default;
    Framebuffer& operator=(Framebuffer&&) = default;

    GuiUnit_t GetWidth() const { return mScreenSurfaces[0].mWidth; }
    GuiUnit_t GetHeight() const { return mScreenSurfaces[0].mHeight; }

    /**
     * \brief Sets a single pixel to the given Color
     * \param aPoint Reference to the coordinate for the pixel to be set
     * \param aColor Reference to the color the pixel is set to
     */
    void SetPixel(GuiUnit_t aX, GuiUnit_t aY, const Color &arColor);
    void SetPixel(const Point &arP, const Color &arColor) { SetPixel(arP.GetX(), arP.GetY(), arColor); }

    /**
     * \brief Gets a single pixel to the given Color
     * \param aPoint Reference to the coordinate for the pixel to get
     * \param aFront Gets pixels from the backbuffer by default, set true to read front buffer
     */
    uint32_t GetPixel(GuiUnit_t aX, GuiUnit_t aY, bool aFront = false) const;
    uint32_t GetPixel(const Point &arP, bool aFront = false) { return GetPixel(arP.GetX(), arP.GetY(), aFront); }

protected:
    GfxHal &mrGfxHal;
    int mFramebufferFile;
    int mTtyFb = 0;
    struct fb_fix_screeninfo mFixedInfo{};
    struct fb_var_screeninfo mVariableInfo{};
    VideoSurface mScreenSurfaces[2]{};
    int mCurrentSurface = 0;

    void swapBuffer();
};

} // namespace rsp::graphics

#endif // FRAMEBUFFER_H
