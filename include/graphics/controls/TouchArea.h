#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "graphics/controls/Image.h"
#include "graphics/controls/Input.h"
#include "graphics/primitives/Rect.h"

namespace rsp::graphics
{

class TouchArea
{
  private:
    Rect mTouchArea;
    // Image *mImage;

  public:
    TouchArea(Rect &aArea /*, Image &aImage*/);
    ~TouchArea();

    void ProcessInput(Input &aInput);
    bool IsHit(const Point &aPoint) const;
    // Register callback method for an Image or simply a Control?
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H