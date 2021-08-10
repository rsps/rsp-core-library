#ifndef PEN_H
#define PEN_H

#include <iostream>

#include "Color.h"

class Pen {
   public:
    int size;
    Color color;

    Pen(int size, Color color);
};

#endif  // PEN_H