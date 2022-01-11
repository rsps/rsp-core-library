#ifndef FRAMEBUFFERCANVAS_H
#define FRAMEBUFFERCANVAS_H

#include <linux/fb.h>

#include "BufferedCanvas.h"
#include "primitives/Canvas.h"

namespace rsp::graphics
{

class Framebuffer : public BufferedCanvas
{
  public:
    Framebuffer();
    virtual ~Framebuffer();

    inline void SetPixel(const Point &aPoint, const Color aColor)
    {
        if (!IsInsideScreen(aPoint)) {
            return;
        }
        long location = (aPoint.mX + mVariableInfo.xoffset) * (mVariableInfo.bits_per_pixel / 8) + aPoint.mY * mFixedInfo.line_length;
        // std::cout << "location:" << location << std::endl;
        *(reinterpret_cast<uint32_t *>(mpBackBuffer + location)) = aColor;
    }

    uint32_t GetPixel(const Point &aPoint, const bool aFront = false) const;

    void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy);

  protected:
    int mFramebufferFile;
    int mTtyFb = 0;
    struct fb_fix_screeninfo mFixedInfo;
    struct fb_var_screeninfo mVariableInfo;

    void clear();
    void copy();

    inline void plot4Points(int aCenterX, int aCenterY, int aX, int aY, const Color &aColor)
    {
        SetPixel(Point(aCenterX + aX, aCenterY + aY), aColor);
        SetPixel(Point(aCenterX - aX, aCenterY + aY), aColor);
        SetPixel(Point(aCenterX + aX, aCenterY - aY), aColor);
        SetPixel(Point(aCenterX - aX, aCenterY - aY), aColor);
    }

    inline void plot8Points(int aCenterX, int aCenterY, int aX, int aY, const Color &aColor)
    {
        plot4Points(aCenterX, aCenterY, aX, aY, aColor);
        plot4Points(aCenterX, aCenterY, aY, aX, aColor);
    }
};

} // namespace rsp::graphics
#endif // FRAMEBUFFERCANVAS_H
