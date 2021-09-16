#ifndef RECT_H
#define RECT_H

#include <utils/RSPCoreExceptions.h>
#include "Point.h"

class Rect {
   public:
    Point LeftTop;
    Point RightBottom;

    Rect(int aLeft, int aTop, int aWidth, int aHeight);
    Rect(const Point &aLeftTop, const Point &aRightBottom);
    Rect(const Point &aLeftTop, int aWidth, int aHeight);
    Rect(const Rect &aRect);

    int GetTop();
    void SetTop(int aTopValue);
    int GetBottom();
    void SetBottom(int aBotValue);
    int GetLeft();
    void SetLeft(int aLeftValue);
    int GetRight();
    void SetRight(int aRightValue);
    int GetWidth();
    void SetWidth(int aWidth);
    int GetHeight();
    void SetHeight(int aHeight);
    bool IsHit(const Point &aPoint);
    bool VerifyDimensions();
};

#endif  // RECT_H
