#ifndef IMAGE_H
#define IMAGE_H

#include "graphics/controls/Control.h"
#include "graphics/primitives/Bitmap.h"

namespace rsp::graphics
{

class Image : public Control
{
  private:
    Bitmap mBitmap;

  public:
    Image();
    ~Image();
};

} // namespace rsp::graphics
#endif // IMAGE_H