/*!
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * \copyright   Copyright 2021 RSP Systems A/S. All rights reserved.
 * \license     Mozilla Public License 2.0
 * \author      Simon Glashoff
 * \author      Steffen Brummer
 */
#ifndef CANVAS_H
#define CANVAS_H

#include "Bitmap.h"
#include "Color.h"
#include "Font.h"
#include "Pen.h"
#include "Point.h"
#include "Rect.h"

namespace rsp::graphics {

class Canvas
{
  public:
    enum class SwapOperations { NoOp,
                                Copy,
                                Clear };

    virtual ~Canvas() {};

    virtual void DrawDot(const Point &aPoint, const Pen &aPen) = 0;
    virtual void DrawArc(const Point &aCenter, int aRadius1, int aRadius2, int aStartAngel, int aSweepAngle, const Pen &aPen) = 0;
    virtual void DrawCircle(const Point &aCenter, int aRadius, const Pen &aPen) = 0;
    virtual void DrawLine(const Point &aA, const Point &aB, const Pen &aPen) = 0;
    virtual void DrawRectangle(const Rect &aRect, const Pen &aPen) = 0;
    virtual void DrawImage(const Point &LeftTop, const Bitmap &aBitmap) = 0;
    virtual void DrawText(const Rect &aRect, const Font &aFont, const char *apText, bool aScaleToFit) = 0;
    virtual void SetPixel(const Point &aPoint, const Color aColor) = 0;
    virtual void SwapBuffer(const SwapOperations aSwapOp = SwapOperations::Copy) = 0;
};

}
#endif //CANVAS_H
