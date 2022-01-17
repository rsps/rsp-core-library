#ifndef TOUCHAREA_H
#define TOUCHAREA_H

#include "graphics/controls/Event.h"
#include "graphics/primitives/Rect.h"

namespace rsp::graphics
{

class TouchArea
{
  private:
    Rect mTouchArea;

  public:
    TouchArea();
    ~TouchArea();

    bool ProcessEvent(Event &aEvent);
    bool IsHit(const Point &aPoint) const;
};

} // namespace rsp::graphics
#endif // TOUCHAREA_H