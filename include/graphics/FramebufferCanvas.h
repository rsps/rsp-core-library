#ifndef FRAMEBUFFERCANVAS_H
#define FRAMEBUFFERCANVAS_H

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "graphics/primitives/Canvas.h"

class Framebuffer : Canvas {
   public:
    int framebufferFile;
    int tty_fb;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    uint8_t *frontBuffer, *backBuffer, *tmp;

    Framebuffer();
    ~Framebuffer();

    void DrawDot(const Point &aPoint, const Pen &aPen);
    void DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen);
    void DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen);
    void DrawLine(const Point &aA, const Point &aB, const Pen &aPen);
    void DrawRectangle(const Rect &aRect, const Pen &aPen);
    void DrawImage(const Point &LeftTop, const Bitmap &aBitmap);
    void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit);
    inline void SetPixel(const Point &aPoint, const Colour aColor) {
        long location = (aPoint.x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + aPoint.y * finfo.line_length;
        //std::cout << "location:" << location << std::endl;
        *((uint32_t *)(backBuffer + location)) = aColor;
    }
    uint32_t GetPixel(const Point &aPoint, const bool &aFront);
    void SwapBuffer();
    void Clear();
};

#endif  // FRAMEBUFFERCANVAS_H