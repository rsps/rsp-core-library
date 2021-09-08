#ifndef PEN_H
#define PEN_H

#include <iostream>

#include "Bitmap.h"
#include "Colour.h"

class Pen {
   public:
    int size;
    Colour colour;
    //Future, pens takes bitmap to be used for drawing
    //Bitmap penType;

    Pen();
    Pen(int aSize, Colour aColour);
};

#endif  // PEN_H