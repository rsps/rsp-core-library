#ifndef CANVAS_H
#define CANVAS_H

#include "Bitmap.h"
#include "Font.h"
#include "Pen.h"
#include "Point.h"
#include "Rect.h"

class Canvas {
   public:
    virtual void DrawDot(const Point &aPoint, const Pen &aPen) = 0;
    virtual void DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen) = 0;
    virtual void DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen) = 0;
    virtual void DrawLine(const Point &aA, const Point &aB, const Pen &aPen) = 0;
    virtual void DrawRectangle(const Rect &aRect, const Pen &aPen) = 0;
    virtual void DrawImage(const Point &LeftTop, const Bitmap &aBitmap) = 0;
    virtual void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit) = 0;
};

#endif  //CANVAS_H