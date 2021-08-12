#ifndef PEN_H
#define PEN_H

#include <iostream>

#include "Colour.h"

class Pen {
   public:
    int size;
    Colour colour;

    Pen(int aSize, Colour aColour);
};

#endif  // PEN_H