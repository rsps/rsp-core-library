/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 */

#include "FramebufferCanvas.h"

Framebuffer::Framebuffer() {
    framebufferFile = open("/dev/fb0", O_RDWR);

    //get fixed screen info
    ioctl(framebufferFile, FBIOGET_VSCREENINFO, &vinfo);

    //get variable screen info
    ioctl(framebufferFile, FBIOGET_FSCREENINFO, &finfo);

    //cout general info
    std::cout << "finfo ypanstep:" << finfo.ypanstep << std::endl;
    std::cout << "finfo xpanstep:" << finfo.xpanstep << std::endl;
    std::cout << "Bits per pixel:" << vinfo.bits_per_pixel << std::endl;

    //set yres_virtual for double buffering
    vinfo.yres_virtual = vinfo.yres * 2;
    if (ioctl(framebufferFile, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        std::cout << "ioctl FBIOPUT_VSCREENINFO failed errno:" << strerror(errno) << std::endl;
    }
    std::cout << "vinfo yres:" << vinfo.yres << std::endl;
    std::cout << "vinfo yres_virtual:" << vinfo.yres_virtual << std::endl;

    //stop the console from drawing ontop of this programs graphics
    tty_fb = open("/dev/tty0", O_RDWR);
    if (ioctl(tty_fb, KDSETMODE, KD_GRAPHICS) == -1) {
        std::cout << "ioctl KDSETMODE KD_GRAPHICS failed errno:" << strerror(errno) << std::endl;
    }

    //calculate size of screen
    long screensize = vinfo.yres * finfo.line_length;
    std::cout << "Screen size:" << screensize << std::endl;

    //map framebuffer to memory
    std::cout << "Mapping framebuffer" << std::endl;
    frontBuffer = static_cast<uint8_t *>(mmap(0, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, framebufferFile, (off_t)0));
    backBuffer = frontBuffer + screensize;
    if (frontBuffer == MAP_FAILED) {
        std::cout << "Mapping failed errno:" << strerror(errno) << std::endl;
    }
}
Framebuffer::~Framebuffer() {
    //At exit we MUST release the tty again
    if (ioctl(tty_fb, KDSETMODE, KD_TEXT) == -1) {
        std::cout << "ioctl KDSETMODE KD_TEXT failed errno:" << strerror(errno) << std::endl;
    }
}

void Framebuffer::DrawDot(const Point &aPoint, const Pen &aPen) {
    throw NotImplementedException("");
}
void Framebuffer::DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen) {
    throw NotImplementedException("");
}
void Framebuffer::DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen) {
    throw NotImplementedException("");
}
void Framebuffer::DrawLine(const Point &aA, const Point &aB, const Pen &aPen) {
    throw NotImplementedException("");
}
void Framebuffer::DrawRectangle(const Rect &aRect, const Pen &aPen) {
    throw NotImplementedException("");
}
void Framebuffer::DrawImage(const Point &LeftTop, const Bitmap &aBitmap) {
    throw NotImplementedException("");
}
void Framebuffer::DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit) {
    throw NotImplementedException("");
}