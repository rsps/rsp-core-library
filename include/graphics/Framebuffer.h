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

#include <graphics/primitives/Rect.h>
#include <graphics/primitives/Texture.h>
#include "BufferedCanvas.h"

namespace rsp::graphics
{

class Framebuffer : public BufferedCanvas
{
  public:
    Framebuffer(const char *apDevPath = nullptr);
    virtual ~Framebuffer();

    void BlitTexture(const Texture &arTexture);

    /**
     * \brief Swaps front and back buffers
     */
    void SwapBuffer() override;

    void Fill(rsp::graphics::Color aColor = Color::Black) override;

    GuiUnit_t GetWidth();
    GuiUnit_t GetHeight();

  protected:
    int mFramebufferFile;
    int mTtyFb = 0;
    struct fb_fix_screeninfo mFixedInfo{};
    struct fb_var_screeninfo mVariableInfo{};
    Rect mClipRect{};

    void clear(Color aColor);
    void copy();
};

} // namespace rsp::graphics
#endif // FRAMEBUFFERCANVAS_H
