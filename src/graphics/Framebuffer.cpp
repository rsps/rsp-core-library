/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <exceptions/ExceptionHelper.h>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <graphics/Framebuffer.h>
#include <iostream>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>
#include <memory>

namespace rsp::graphics
{

const char* Framebuffer::mpDevicePath = nullptr;

Framebuffer::Framebuffer()
    : mrGfxHal(GfxHal::Get()),
      mFramebufferFile(-1)
{
    if (mpDevicePath) {
        mFramebufferFile = open(mpDevicePath, O_RDWR);
        if (mFramebufferFile == -1) {
            std::clog << "Failed to open framebuffer at " << mpDevicePath << " trying /dev/fb0" << std::endl;
        }
    }
    if (mFramebufferFile == -1) {
        mFramebufferFile = open("/dev/fb0", O_RDWR);
        if (mFramebufferFile == -1) {
            THROW_SYSTEM("Failed to open framebuffer");
        }
    }

    // get fixed screen info
    ioctl(mFramebufferFile, FBIOGET_VSCREENINFO, &mVariableInfo);

    // get variable screen info
    ioctl(mFramebufferFile, FBIOGET_FSCREENINFO, &mFixedInfo);

    mScreenSurfaces[0].mWidth = static_cast<GuiUnit_t>(mVariableInfo.xres);
    mScreenSurfaces[0].mHeight = static_cast<GuiUnit_t>(mVariableInfo.yres);
    mScreenSurfaces[0].mRowPitch = static_cast<uintptr_t>(mFixedInfo.line_length);
    mScreenSurfaces[1].mWidth    = mScreenSurfaces[0].mWidth;
    mScreenSurfaces[1].mHeight   = mScreenSurfaces[0].mHeight;
    mScreenSurfaces[1].mRowPitch = mScreenSurfaces[0].mRowPitch;

    // std::clog << "Framebuffer opened. Width=" << mWidth << " Height=" << mHeight << " BytesPerPixel=" << mBytesPerPixel << std::endl;

    // set yres_virtual for double buffering
    mVariableInfo.yres_virtual = mVariableInfo.yres * 2;
    if (ioctl(mFramebufferFile, FBIOPUT_VSCREENINFO, &mVariableInfo) == -1) {
        THROW_SYSTEM("Framebuffer ioctl FBIOPUT_VSCREENINFO failed");
    }

    // stop the console from drawing ontop of this programs graphics
    if (access("/dev/tty0", O_RDWR) == 0) {
        mTtyFb = open("/dev/tty0", O_RDWR);
        if (ioctl(mTtyFb, KDSETMODE, KD_GRAPHICS) == -1) {
            THROW_SYSTEM("Framebuffer ioctl KDSETMODE KD_GRAPHICS failed");
        }
    }

    // calculate size of screen
    std::size_t screensize = mVariableInfo.yres * mFixedInfo.line_length;

    uint32_t *fb = static_cast<uint32_t *>(mmap(nullptr, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, mFramebufferFile, static_cast<off_t>(0)));
    if (uintptr_t(fb) == uintptr_t(-1)) /*MAP_FAILED*/ {
        THROW_SYSTEM("Framebuffer shared memory mapping failed");
    }

    mScreenSurfaces[0].mpVirtAddr = VideoSurface::PixelPtr_t(fb, [screensize](uint32_t *p) noexcept {
        munmap(p, screensize*2);
    });
    mScreenSurfaces[1].mpVirtAddr = VideoSurface::PixelPtr_t(fb + (screensize / sizeof(std::uint32_t)), [](uint32_t apPtr[]) noexcept {});

    if (mVariableInfo.yoffset == 0) {
        mCurrentSurface = 1;
    }
}

Framebuffer::~Framebuffer()
{
    // At exit we MUST release the tty again
    if ((mTtyFb > 0) &&
        (ioctl(mTtyFb, KDSETMODE, KD_TEXT) == -1)) {
        std::cout << "ioctl KDSETMODE KD_TEXT failed errno:" << strerror(errno) << std::endl;
    }
    // Close the handle to the framebuffer device
    if (mFramebufferFile > 0) {
        close(mFramebufferFile);
    }
}

void Framebuffer::swapBuffer()
{
    // swap buffer
    if (mVariableInfo.yoffset == 0) {
        mVariableInfo.yoffset = mVariableInfo.yres;
        mCurrentSurface = 0;
    } else {
        mVariableInfo.yoffset = 0;
        mCurrentSurface = 1;
    }

    // Sync to next vblank
    mVariableInfo.activate = FB_ACTIVATE_VBL;

    // Pan to back buffer
    if (ioctl(mFramebufferFile, FBIOPAN_DISPLAY, &mVariableInfo) == -1) {
        THROW_SYSTEM("ioctl FBIOPAN_DISPLAY failed");
    }
}

} // namespace rsp::graphics
