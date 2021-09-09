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
    inline void plot4Points(int aCenterX, int aCenterY, int aX, int aY, const Pen &aPen) {
        DrawDot(Point(aCenterX + aX, aCenterY + aY), aPen);
        DrawDot(Point(aCenterX - aX, aCenterY + aY), aPen);
        DrawDot(Point(aCenterX + aX, aCenterY - aY), aPen);
        DrawDot(Point(aCenterX - aX, aCenterY - aY), aPen);
    }
    inline void plot8Points(int aCenterX, int aCenterY, int aX, int aY, const Pen &aPen) {
        plot4Points(aCenterX, aCenterY, aX, aY, aPen);
        plot4Points(aCenterX, aCenterY, aY, aX, aPen);
    }
    void DrawLine(const Point &aA, const Point &aB, const Pen &aPen);
    void DrawRectangle(const Rect &aRect, const Pen &aPen);
    void DrawImage(const Point &LeftTop, const Bitmap &aBitmap);
    void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit);
    inline void SetPixel(const Point &aPoint, const Colour aColor) {
        if (!IsInsideScreen(aPoint)) {
            return;
        }
        long location = (aPoint.x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + aPoint.y * finfo.line_length;
        //std::cout << "location:" << location << std::endl;
        *((uint32_t *)(backBuffer + location)) = aColor;
    }
    uint32_t GetPixel(const Point &aPoint, const bool aFront = false) const;
    inline bool IsInsideScreen(const Point &aPoint) const {
        return !(aPoint.x < 0 || aPoint.y < 0 || aPoint.y >= vinfo.yres || aPoint.x >= vinfo.xres);
    }
    void SwapBuffer();
    void Clear();
};

#endif  // FRAMEBUFFERCANVAS_H