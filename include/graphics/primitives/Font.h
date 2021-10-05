#ifndef FONT_H
#define FONT_H

#include <graphics/primitives/Color.h>

namespace rsp::graphics {

class Font
{
  public:
    std::string mName;
    std::string mStyle;
    Color mColor;
    int mWeigth;
    int mSize;

    Font(/* args */);
    ~Font();
};

}

#endif //FONT_H
