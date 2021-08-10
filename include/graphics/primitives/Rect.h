#ifndef RECT_H
#define RECT_H

#include "../../src/utils/RSPCoreExceptions.h"
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
    void SetTop(int aValue);
    int GetBottom();
    void SetBottom(int aValue);
    int GetLeft();
    void SetLeft(int aValue);
    int GetRight();
    void SetRight(int aValue);
    int GetWidth();
    void SetWidth(int aValue);
    int GetHeight();
    void SetHeight(int aValue);
    bool IsHit(const Point &aPoint);
};

#endif  // RECT_H