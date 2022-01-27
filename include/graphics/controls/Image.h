#ifndef IMAGE_H
#define IMAGE_H

#include "graphics/controls/Control.h"
#include "graphics/primitives/Bitmap.h"

namespace rsp::graphics
{

class Image : public Control
{
  public:
    Image(std::string aNormal, std::string aPressed, Rect &aRect);
    ~Image();

    void Render(Canvas &aCanvas) override;

    Bitmap mNormal;
    Bitmap mPressed;
};

} // namespace rsp::graphics
#endif // IMAGE_H