#ifndef FONT_H
#define FONT_H

#include <graphics/primitives/Color.h>

namespace rsp::graphics
{

class Font
{
public:
    Font(/* args */);
    ~Font();

protected:
    std::string mName;
    std::string mStyle;
    Color mColor;
    int mWeigth;
    int mSize;
};

}

#endif //FONT_H
