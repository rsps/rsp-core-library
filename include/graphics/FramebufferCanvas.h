#ifndef FRAMEBUFFERCANVAS_H
#define FRAMEBUFFERCANVAS_H

#include <linux/fb.h>

#include "primitives/Canvas.h"

namespace rsp::graphics
{

class Framebuffer : public Canvas
{
  public:
    /**
     * Enumerated SwapOperations
     *
     * In case of double buffer support, these values can control
     * the content of the new buffer:
     *  NoOp:  No initialization of the buffer is performed
     *  Copy:  The current view content is copied int new buffer
     *  Clear: The new buffer is filled with the background color.
     */
    enum class SwapOperations {
        NoOp,
        Copy,
        Clear
    };

    Framebuffer();
    virtual ~Framebuffer();

    void DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Color &aColor);
    void DrawCircle(const Point &aCenter, int aRadius, const Color &aColor);
    void DrawLine(const Point &aA, const Point &aB, const Color &aColor);
    void DrawRectangle(const Rect &aRect, const Color &aColor);
    void DrawImage(const Point &aLeftTop, const Bitmap &aBitmap);
    void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit);
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

    uint32_t GetWidth() const
    {
        return mVariableInfo.xres;
    }
    uint32_t GetHeight() const
    {
        return mVariableInfo.yres;
    }

    uint32_t GetColorDepth() const
    {
        return mVariableInfo.bits_per_pixel;
    }

    inline bool IsInsideScreen(const Point &aPoint) const
    {
        return !(aPoint.mX < 0 || aPoint.mY < 0 || static_cast<uint32_t>(aPoint.mY) >= mVariableInfo.yres || static_cast<uint32_t>(aPoint.mX) >= mVariableInfo.xres);
    }

  protected:
    int mFramebufferFile;
    int mTtyFb = 0;
    struct fb_fix_screeninfo mFixedInfo;
    struct fb_var_screeninfo mVariableInfo;
    uint8_t *mpFrontBuffer;
    uint8_t *mpBackBuffer;

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
