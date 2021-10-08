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
#include <fcntl.h>
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

    //get fixed screen info
    ioctl(mFramebufferFile, FBIOGET_VSCREENINFO, &mVariableInfo);

    //get variable screen info
    ioctl(mFramebufferFile, FBIOGET_FSCREENINFO, &mFixedInfo);

    //set yres_virtual for double buffering
    mVariableInfo.yres_virtual = mVariableInfo.yres * 2;
    if (ioctl(mFramebufferFile, FBIOPUT_VSCREENINFO, &mVariableInfo) == -1) {
        std::cout << "ioctl FBIOPUT_VSCREENINFO failed errno:" << strerror(errno) << std::endl;
    }

    //stop the console from drawing ontop of this programs graphics
    if (access("/dev/tty0", O_RDWR) == 0) {
        mTtyFb = open("/dev/tty0", O_RDWR);
        if (ioctl(mTtyFb, KDSETMODE, KD_GRAPHICS) == -1) {
            std::cout << "ioctl KDSETMODE KD_GRAPHICS failed errno:" << strerror(errno) << std::endl;
        }
    }

    //calculate size of screen
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
    //At exit we MUST release the tty again
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

void Framebuffer::DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen)
{
    throw rsp::utils::NotImplementedException("");
}

void Framebuffer::DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen)
{
    int error = -aRadius;
    //int x = aRadius;
    int y = 0;

    while (aRadius >= y) {
        plot8Points(aCenter.mX, aCenter.mY, aRadius, y, aPen);
        error += y;
        y++;
        error += y;

        if (error >= 0) {
            error += -aRadius;
            aRadius--;
            error += -aRadius;
        }
    }
}

void Framebuffer::DrawLine(const Point &aA, const Point &aB, const Pen &aPen)
{
    int i, x, y, deltaX, deltaY, absDeltaX, absDeltaY, signumX, signumY, px, py;

    deltaX = aB.mX - aA.mX;
    deltaY = aB.mY - aA.mY;
    absDeltaX = abs(deltaX);
    absDeltaY = abs(deltaY);
    signumX = (deltaX > 0) ? 1 : -1;
    signumY = (deltaY > 0) ? 1 : -1;
    x = absDeltaX >> 1;
    y = absDeltaY >> 1;
    px = aA.mX;
    py = aA.mY;

    aPen.Draw(*this, aA);
    if (absDeltaX >= absDeltaY) {
        for (i = 0; i < absDeltaX; i++) {
            y += absDeltaY;
            if (y >= absDeltaX) {
                y -= absDeltaX;
                py += signumY;
            }
            px += signumX;
            aPen.Draw(*this, Point(px, py));
        }
    } else {
        for (i = 0; i < absDeltaY; i++) {
            x += absDeltaX;
            if (x >= absDeltaY) {
                x -= absDeltaY;
                px += signumX;
            }
            py += signumY;
            aPen.Draw(*this, Point(px, py));
        }
    }
}

void Framebuffer::DrawRectangle(const Rect &aRect, const Pen &aPen)
{
    for (int i = aRect.mLeftTop.mX; i <= aRect.mRightBottom.mX; i++) {
        aPen.Draw(*this, Point(i, aRect.mLeftTop.mY));     //top
        aPen.Draw(*this, Point(i, aRect.mRightBottom.mY)); //bottom
    }
    for (int i = aRect.mLeftTop.mY; i <= aRect.mRightBottom.mY; i++) {
        aPen.Draw(*this, Point(aRect.mLeftTop.mX, i));     //left
        aPen.Draw(*this, Point(aRect.mRightBottom.mX, i)); //right
    }
}

void Framebuffer::DrawImage(const Point &aLeftTop, const Bitmap &aBitmap)
{
    int iter = 0;
    auto pixels = aBitmap.GetPixels();
    for (size_t h = 0; h < aBitmap.GetHeight(); h++) {
        for (size_t w = 0; w < aBitmap.GetWidth(); w++) {
            SetPixel(Point(aLeftTop.mX + w, aLeftTop.mY + h), pixels[iter]);
            iter++;
        }
    }
}

void Framebuffer::DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit)
{
    throw rsp::utils::NotImplementedException("Draw Text is not yet implemented");
}

void Framebuffer::SwapBuffer(const SwapOperations aSwapOp)
{

    mVariableInfo.reserved[0]++;

    //swap buffer
    if (mVariableInfo.yoffset == 0) {
        mVariableInfo.yoffset = mVariableInfo.yres;
    } else {
        mVariableInfo.yoffset = 0;
    }
    //Pan to back buffer
    if (ioctl(mFramebufferFile, FBIOPAN_DISPLAY, &mVariableInfo) == -1) {
        std::cout << "ioctl FBIOPAN_DISPLAY failed errno:" << strerror(errno) << std::endl;
    }

    //update pointers
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
    //std::cout << "location:" << location << std::endl;
    if (aFront) {
        return *(reinterpret_cast<uint32_t *>(mpFrontBuffer + location));
    } else {
        return *(reinterpret_cast<uint32_t *>(mpBackBuffer + location));
    }
}

void Framebuffer::clear()
{
    long x, y;
    //draw to back buffer
    //    std::cout << "Clearing buffer" << std::endl;
    for (y = 0; y < mVariableInfo.yres; y++) {
        for (x = 0; x < mVariableInfo.xres; x++) {
            long location = (x + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + y * mFixedInfo.line_length;
            //std::cout << "location:" << location << std::endl;
            *(reinterpret_cast<uint32_t *>(mpBackBuffer + location)) = 0x00000000;
        }
    }
}

void Framebuffer::copy()
{
    long x, y;
    //copy front buffer to back buffer
    //    std::cout << "Copying buffer" << std::endl;
    for (y = 0; y < mVariableInfo.yres; y++) {
        for (x = 0; x < mVariableInfo.xres; x++) {
            long location = (x + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + y * mFixedInfo.line_length;
            //std::cout << "location:" << location << std::endl;
            *(reinterpret_cast<uint32_t *>(mpBackBuffer + location)) = *(reinterpret_cast<uint32_t *>(mpFrontBuffer + location));
        }
    }
}

} // namespace rsp::graphics
