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
#include "graphics/primitives/Color.h"

namespace rsp::graphics
{


class Framebuffer : Canvas
{
  public:
    Framebuffer();
    virtual ~Framebuffer();

    void DrawDot(const Point &aPoint, const Pen &aPen);
    void DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen);
    void DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen);
    inline void plot4Points(int aCenterX, int aCenterY, int aX, int aY, const Pen &aPen)
    {
        aPen.Draw(*this, Point(aCenterX + aX, aCenterY + aY));
        aPen.Draw(*this, Point(aCenterX - aX, aCenterY + aY));
        aPen.Draw(*this, Point(aCenterX + aX, aCenterY - aY));
        aPen.Draw(*this, Point(aCenterX - aX, aCenterY - aY));
    }

    inline void plot8Points(int aCenterX, int aCenterY, int aX, int aY, const Pen &aPen)
    {
        plot4Points(aCenterX, aCenterY, aX, aY, aPen);
        plot4Points(aCenterX, aCenterY, aY, aX, aPen);
    }
    virtual void DrawLine(const Point &aA, const Point &aB, const Pen &aPen);
    void DrawRectangle(const Rect &aRect, const Pen &aPen);
    void DrawImage(const Point &aLeftTop, const Bitmap &aBitmap);
    void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit);
    inline void SetPixel(const Point &aPoint, const Color aColor)
    {
        if (!IsInsideScreen(aPoint)) {
            return;
        }
        long location = (aPoint.mX + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + aPoint.mY * finfo.line_length;
        //std::cout << "location:" << location << std::endl;
        *(reinterpret_cast<uint32_t*>(backBuffer + location)) = aColor;
    }

    uint32_t GetPixel(const Point &aPoint, const bool aFront = false) const;
    inline bool IsInsideScreen(const Point &aPoint) const
    {
        return !(aPoint.mX < 0 || aPoint.mY < 0 || aPoint.mY >= vinfo.yres || aPoint.mX >= vinfo.xres);
    }

    void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy);

    uint32_t GetWidth() const {
        return vinfo.xres;
    }
    uint32_t GetHeight() const {
        return vinfo.yres;
    }

    uint32_t GetColorDepth() const {
        return vinfo.bits_per_pixel;
    }


  protected:
    int framebufferFile;
    int tty_fb = 0;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    uint8_t *frontBuffer, *backBuffer;

    void clear();
    void copy();
};

}
#endif // FRAMEBUFFERCANVAS_H
