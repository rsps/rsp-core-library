#ifndef PEN_H
#define PEN_H

#include <iostream>

#include "Bitmap.h"
#include "Colour.h"
#include "Point.h"

class Canvas;

class Pen
{
  public:
    int mSize;
    Colour mColour;

    Pen();
    Pen(int aSize, Colour aColour);
    void Draw(Canvas &aCanvas, const Point &aPoint) const;
};

#endif // PEN_H