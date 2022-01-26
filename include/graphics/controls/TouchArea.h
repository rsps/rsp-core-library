#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "graphics/controls/Event.h"
#include "graphics/controls/Image.h"
#include "graphics/primitives/Rect.h"

namespace rsp::graphics
{

class TouchArea
{
  private:
    Rect mTouchArea;
    Image mImage;

  public:
    TouchArea(Rect &aArea, Image &aImage);
    ~TouchArea();

    bool ProcessEvent(Event &aEvent);
    bool IsHit(const Point &aPoint) const;
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H