/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include <graphics/FramebufferCanvas.h>

#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>

namespace rsp::graphics
{

Framebuffer::Framebuffer()
{
    mFramebufferFile = open("/dev/fb1", O_RDWR);
    if (mFramebufferFile == -1) {
        mFramebufferFile = open("/dev/fb0", O_RDWR);
        if (mFramebufferFile == -1) {
            throw std::system_error(errno, std::generic_category(), "Failed to open framebuffer");
        }
    }

    // get fixed screen info
    ioctl(mFramebufferFile, FBIOGET_VSCREENINFO, &mVariableInfo);

    // get variable screen info
    ioctl(mFramebufferFile, FBIOGET_FSCREENINFO, &mFixedInfo);

    // set Canvas specific variables
    mWidth = mVariableInfo.yres;
    mHeight = mVariableInfo.xres;
    mBytesPerPixel = mVariableInfo.bits_per_pixel / 8;

    // set yres_virtual for double buffering
    mVariableInfo.yres_virtual = mVariableInfo.yres * 2;
    if (ioctl(mFramebufferFile, FBIOPUT_VSCREENINFO, &mVariableInfo) == -1) {
        std::cout << "ioctl FBIOPUT_VSCREENINFO failed errno:" << strerror(errno) << std::endl;
    }

    // stop the console from drawing ontop of this programs graphics
    if (access("/dev/tty0", O_RDWR) == 0) {
        mTtyFb = open("/dev/tty0", O_RDWR);
        if (ioctl(mTtyFb, KDSETMODE, KD_GRAPHICS) == -1) {
            std::cout << "ioctl KDSETMODE KD_GRAPHICS failed errno:" << strerror(errno) << std::endl;
        }
    }

    // calculate size of screen
    long screensize = mVariableInfo.yres * mFixedInfo.line_length;

    mpFrontBuffer = static_cast<uint8_t *>(mmap(0, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, mFramebufferFile, static_cast<off_t>(0)));
    if (mpFrontBuffer == reinterpret_cast<uint8_t *>(-1)) /*MAP_FAILED*/ {
        std::cout << "Mapping failed errno:" << strerror(errno) << std::endl;
    }

    mpBackBuffer = mpFrontBuffer + screensize;

    if (mVariableInfo.yoffset > 0) {
        uint8_t *tmp = mpFrontBuffer;
        mpFrontBuffer = mpBackBuffer;
        mpBackBuffer = tmp;
    }
}

Framebuffer::~Framebuffer()
{
    // At exit we MUST release the tty again
    if (mTtyFb > 0) {
        if (ioctl(mTtyFb, KDSETMODE, KD_TEXT) == -1) {
            std::cout << "ioctl KDSETMODE KD_TEXT failed errno:" << strerror(errno) << std::endl;
        }
    }
    // Close the handle to the framebuffer device
    if (mFramebufferFile > 0) {
        close(mFramebufferFile);
    }
    // No need to call munmap on the shared memory region, this is done automatically on termination.
}

void Framebuffer::SwapBuffer(const SwapOperations aSwapOp)
{
    // swap buffer
    if (mVariableInfo.yoffset == 0) {
        mVariableInfo.yoffset = mVariableInfo.yres;
    } else {
        mVariableInfo.yoffset = 0;
    }
    // Pan to back buffer
    if (ioctl(mFramebufferFile, FBIOPAN_DISPLAY, &mVariableInfo) == -1) {
        std::cout << "ioctl FBIOPAN_DISPLAY failed errno:" << strerror(errno) << std::endl;
    }

    // update pointers
    uint8_t *tmp = mpFrontBuffer;
    mpFrontBuffer = mpBackBuffer;
    mpBackBuffer = tmp;

    switch (aSwapOp) {
    case SwapOperations::Copy:
        copy();
        break;

    case SwapOperations::Clear:
        clear();
        break;

    case SwapOperations::NoOp:
    default:
        break;
    }
}

uint32_t Framebuffer::GetPixel(const Point &aPoint, const bool aFront) const
{
    if (!IsInsideScreen(aPoint)) {
        return 0;
    }
    long location = (aPoint.mX + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + aPoint.mY * mFixedInfo.line_length;
    // std::cout << "location:" << location << std::endl;
    if (aFront) {
        return *(reinterpret_cast<uint32_t *>(mpFrontBuffer + location));
    } else {
        return *(reinterpret_cast<uint32_t *>(mpBackBuffer + location));
    }
}

void Framebuffer::clear()
{
    long x, y;
    // draw to back buffer
    //     std::cout << "Clearing buffer" << std::endl;
    for (y = 0; y < mVariableInfo.yres; y++) {
        for (x = 0; x < mVariableInfo.xres; x++) {
            long location = (x + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + y * mFixedInfo.line_length;
            // std::cout << "location:" << location << std::endl;
            *(reinterpret_cast<uint32_t *>(mpBackBuffer + location)) = 0x00000000;
        }
    }
}

void Framebuffer::copy()
{
    long x, y;
    // copy front buffer to back buffer
    //     std::cout << "Copying buffer" << std::endl;
    for (y = 0; y < mVariableInfo.yres; y++) {
        for (x = 0; x < mVariableInfo.xres; x++) {
            long location = (x + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + y * mFixedInfo.line_length;
            // std::cout << "location:" << location << std::endl;
            *(reinterpret_cast<uint32_t *>(mpBackBuffer + location)) = *(reinterpret_cast<uint32_t *>(mpFrontBuffer + location));
        }
    }
}

} // namespace rsp::graphics
