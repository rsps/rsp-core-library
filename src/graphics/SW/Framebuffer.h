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
#ifndef RSP_CORE_LIB_SRC_GRAPHICS_SW_FRAMEBUFFER_H
#define RSP_CORE_LIB_SRC_GRAPHICS_SW_FRAMEBUFFER_H

#ifdef USE_GFX_SW

#include <graphics/Rect.h>
#include <linux/fb.h>
#include "GfxHal.h"


namespace rsp::graphics::sw {

class Framebuffer
{
public:
    static std::string mDevicePath;

    Framebuffer();
    virtual ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = default;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;

    GuiUnit_t GetWidth() const { return mScreenSurfaces[0].mWidth; }
    GuiUnit_t GetHeight() const { return mScreenSurfaces[0].mHeight; }

protected:
    GfxHal &mrGfxHal;
    int mFramebufferFile;
    int mTtyFb = 0;
    struct fb_fix_screeninfo mFixedInfo{};
    struct fb_var_screeninfo mVariableInfo{};
    mutable VideoSurface mScreenSurfaces[2]{}; // Double buffers
    int mCurrentSurface = 0; // Index of current back buffer

    void swapBuffer();
};

} // namespace rsp::graphics::sw

#endif /* USE_GFX_SW */
#endif // RSP_CORE_LIB_SRC_GRAPHICS_SW_FRAMEBUFFER_H
