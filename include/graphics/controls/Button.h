#ifndef BUTTON_H
#define BUTTON_H

#include "graphics/controls/Image.h"
#include "graphics/controls/Label.h"
#include "graphics/controls/TouchArea.h"

namespace rsp::graphics
{

class Button
{
  private:
    TouchArea mTouchArea;
    Label mLabel;
    Image mNormal;
    Image mPressed;
    Image mInactive;

  public:
    Button();
    ~Button();
};

} // namespace rsp::graphics
#endif // BUTTON_H