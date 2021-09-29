#ifndef FONT_H
#define FONT_H

#include "Colour.h"

class Font
{
  public:
    std::string mName;
    std::string mStyle;
    Colour mColour;
    int mWeigth;
    int mSize;

    Font(/* args */);
    ~Font();
};

#endif //FONT_H