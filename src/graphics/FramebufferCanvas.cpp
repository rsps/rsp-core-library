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
    //framebufferFile = open("/dev/fb0", O_RDWR);
    framebufferFile = open("/dev/fb1", O_RDWR);
    if (framebufferFile == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to open framebuffer");
    }

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
    vinfo.yoffset = 0;
    if (ioctl(framebufferFile, FBIOPUT_VSCREENINFO, &vinfo) == -1) {
        std::cout << "ioctl FBIOPUT_VSCREENINFO failed errno:" << strerror(errno) << std::endl;
    }
    std::cout << "vinfo yres:" << vinfo.yres << std::endl;
    std::cout << "vinfo yres_virtual:" << vinfo.yres_virtual << std::endl;

    //stop the console from drawing ontop of this programs graphics
    if (access("/dev/tty0", O_RDWR) == 0) {
        tty_fb = open("/dev/tty0", O_RDWR);
        if (ioctl(tty_fb, KDSETMODE, KD_GRAPHICS) == -1) {
            std::cout << "ioctl KDSETMODE KD_GRAPHICS failed errno:" << strerror(errno) << std::endl;
        }
    }

    //calculate size of screen
    long screensize = vinfo.yres * finfo.line_length;
    std::cout << "Screen size:" << screensize << std::endl;

    //map framebuffer to memory
    std::cout << "Mapping framebuffer" << std::endl;
    frontBuffer = static_cast<uint8_t *>(mmap(0, screensize * 2, PROT_READ | PROT_WRITE, MAP_SHARED, framebufferFile, (off_t)0));
    if (frontBuffer == MAP_FAILED) {
        std::cout << "Mapping failed errno:" << strerror(errno) << std::endl;
    }
    backBuffer = frontBuffer + screensize;
}
Framebuffer::~Framebuffer() {
    //At exit we MUST release the tty again
    if (tty_fb > 0) {
        if (ioctl(tty_fb, KDSETMODE, KD_TEXT) == -1) {
            std::cout << "ioctl KDSETMODE KD_TEXT failed errno:" << strerror(errno) << std::endl;
        }
    }
}

void Framebuffer::DrawDot(const Point &aPoint, const Pen &aPen) {
    //Check if we are outside screen
    if (!IsInsideScreen(aPoint)) {
        return;
    }
    //In the future use the Pen bitmap to draw
    long location = (aPoint.x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + aPoint.y * finfo.line_length;
    //std::cout << "location:" << location << std::endl;
    *((uint32_t *)(backBuffer + location)) = aPen.colour;
}
void Framebuffer::DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen) {
    throw NotImplementedException("");
}
void Framebuffer::DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen) {
    int error = -aRadius;
    //int x = aRadius;
    int y = 0;

    while (aRadius >= y) {
        plot8Points(aCenter.x, aCenter.y, aRadius, y, aPen);
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
void Framebuffer::DrawLine(const Point &aA, const Point &aB, const Pen &aPen) {
    int i, x, y, deltaX, deltaY, absDeltaX, absDeltaY, signumX, signumY, px, py;

    deltaX = aB.x - aA.x;
    deltaY = aB.y - aA.y;
    absDeltaX = abs(deltaX);
    absDeltaY = abs(deltaY);
    signumX = (deltaX > 0) ? 1 : -1;
    signumY = (deltaY > 0) ? 1 : -1;
    x = absDeltaX >> 1;
    y = absDeltaY >> 1;
    px = aA.x;
    py = aA.y;

    DrawDot(aA, aPen);
    if (absDeltaX >= absDeltaY) {
        for (i = 0; i < absDeltaX; i++) {
            y += absDeltaY;
            if (y >= absDeltaX) {
                y -= absDeltaX;
                py += signumY;
            }
            px += signumX;
            DrawDot(Point(px, py), aPen);
        }
    } else {
        for (i = 0; i < absDeltaY; i++) {
            x += absDeltaX;
            if (x >= absDeltaY) {
                x -= absDeltaY;
                px += signumX;
            }
            py += signumY;
            DrawDot(Point(px, py), aPen);
        }
    }
}
void Framebuffer::DrawRectangle(const Rect &aRect, const Pen &aPen) {
    for (int i = aRect.LeftTop.x; i <= aRect.RightBottom.x; i++) {
        DrawDot(Point(i, aRect.LeftTop.y), aPen);      //top
        DrawDot(Point(i, aRect.RightBottom.y), aPen);  //bottom
    }
    for (int i = aRect.LeftTop.y; i <= aRect.RightBottom.y; i++) {
        DrawDot(Point(aRect.LeftTop.x, i), aPen);      //left
        DrawDot(Point(aRect.RightBottom.x, i), aPen);  //right
    }
}
void Framebuffer::DrawImage(const Point &LeftTop, const Bitmap &aBitmap) {
    throw NotImplementedException("Draw Image is not yet implemented");
}
void Framebuffer::DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit) {
    throw NotImplementedException("Draw Text is not yet implemented");
}
void Framebuffer::SwapBuffer() {
    std::cout << "Swapping buffer: " << vinfo.yoffset << std::endl;
    //swap buffer
    if (vinfo.yoffset == 0) {
        vinfo.yoffset = vinfo.yres;
    } else {
        vinfo.yoffset = 0;
    }
    //Pan to back buffer
    if (ioctl(framebufferFile, FBIOPAN_DISPLAY, &vinfo) == -1) {
        std::cout << "ioctl FBIOPAN_DISPLAY failed errno:" << strerror(errno) << std::endl;
    }

    //update pointers
    uint8_t *tmp = frontBuffer;
    frontBuffer = backBuffer;
    backBuffer = tmp;

    Clear();
}
uint32_t Framebuffer::GetPixel(const Point &aPoint, const bool aFront) const {
    if (!IsInsideScreen(aPoint)) {
        return 0;
    }
    long location = (aPoint.x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + aPoint.y * finfo.line_length;
    //std::cout << "location:" << location << std::endl;
    if (aFront) {
        return *((uint32_t *)(frontBuffer + location));
    } else {
        return *((uint32_t *)(backBuffer + location));
    }
}
void Framebuffer::Clear() {
    long x, y;
    //draw to back buffer
    for (y = 0; y < vinfo.yres; y++) {
        for (x = 0; x < vinfo.xres; x++) {
            long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + y * finfo.line_length;
            //std::cout << "location:" << location << std::endl;
            *((uint32_t *)(backBuffer + location)) = 0x000000;
        }
    }
}