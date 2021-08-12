#ifndef FONT_H
#define FONT_H

#include "Colour.h"

class Font {
   public:
    std::string name;
    std::string style;
    Colour colour;
    int weigth;
    int size;

    Font(/* args */);
    ~Font();
};

#endif  //FONT_H